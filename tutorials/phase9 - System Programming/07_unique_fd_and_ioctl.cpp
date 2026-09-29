/**
 * Phase 9-7: RAII unique_fd & Kernel Driver ioctl Model
 *
 * 在 Android Native / HAL 開發（例如開啟 /dev/video0、/dev/lwis-*、DMA-BUF fd、Sync Fence fd）中：
 *   1. 裸 int fd 非常容易在提早 return 時忘記 close() 造成 fd leak（Android 行程預設 fd 上限常為 1024~32768）。
 *      因此 AOSP <android-base/unique_fd.h> 提供了 move-only 的 RAII `unique_fd`。
 *   2. read() / write() 只適合純 byte stream；面對硬體驅動（設定曝光、查詢感測器資訊、佇列 buffer），
 *      Linux 使用 `ioctl(fd, request_cmd, &struct_payload)` 作為控制通道。
 *
 * 目錄:
 *   1. 手寫 move-only RAII unique_fd（對標 android::base::unique_fd）
 *   2. unique_fd 的轉移 (std::move)、重置 (reset) 與釋放所有權 (release)
 *   3. Linux Driver ioctl 協定模型（_IOR / _IOW / _IOWR 編碼概念與模擬）
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <utility>
using namespace std;

// ============================================================
// 1. RAII unique_fd（對標 android::base::unique_fd）
// ============================================================
// 本章重點：
//   - 無效的指標是 nullptr，但無效的 fd 是 -1！
//   - 不能複製（否則兩個 unique_fd 解構會 close 同一個 fd 兩次，甚至關掉別的執行緒剛開的新 fd！）
//   - Move 之後必須把來源的 fd_ 設回 -1。
class UniqueFd {
public:
    UniqueFd() = default;
    explicit UniqueFd(int fd) : fd_(fd) {}

    ~UniqueFd() { reset(); }

    // 禁止 copy
    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    // 允許 move
    UniqueFd(UniqueFd&& other) noexcept : fd_(other.release()) {}

    UniqueFd& operator=(UniqueFd&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    int get() const { return fd_; }
    bool ok() const { return fd_ >= 0; }

    // 交出所有權，呼叫者自己負責 close（常見於跨 Binder / C API 傳遞 fd）
    int release() noexcept {
        return std::exchange(fd_, -1);
    }

    // 關掉舊 fd，換成新 fd
    void reset(int new_fd = -1) noexcept {
        if (fd_ >= 0) {
            close(fd_);
        }
        fd_ = new_fd;
    }

private:
    int fd_ = -1;
};

void unique_fd_demo() {
    // Output:
    // === RAII unique_fd ===
    //   opened fd ok = true
    //   after move: fd1.ok = false, fd2.ok = true
    //   leaving scope -> fd2 automatically closed
    //
    cout << "=== RAII unique_fd ===" << endl;

    {
        UniqueFd fd1(open("/dev/null", O_RDWR));
        cout << "  opened fd ok = " << boolalpha << fd1.ok() << endl;

        UniqueFd fd2 = std::move(fd1);
        cout << "  after move: fd1.ok = " << fd1.ok()
             << ", fd2.ok = " << fd2.ok() << noboolalpha << endl;
        cout << "  leaving scope -> fd2 automatically closed" << endl;
    }
    cout << endl;
}

// ============================================================
// 2. Linux Driver ioctl 模型（User-space HAL <-> Kernel Driver）
// ============================================================
// 本章重點：
//   在 Android Camera HAL (V4L2 / LWIS) 中，User-space 透過 ioctl 與 Kernel 交換固定格式的 struct：
//     - _IOR(type, nr, struct_type)  : User 從 Driver 讀取資料
//     - _IOW(type, nr, struct_type)  : User 寫入設定給 Driver
//     - _IOWR(type, nr, struct_type) : 雙向交換（例如傳入 register address，Driver 填回 value）
//
//   注意：傳進 ioctl 的 struct 不能含有 std::string 或 std::vector（因為 Kernel 無法解參考 User-space C++ 物件），
//   必須是固定長度的 POD / standard-layout struct（或搭配明確的 user pointer + length）。
struct SensorRegIo {
    uint16_t reg_addr;
    uint16_t reg_val;
    uint32_t status;
};

// 使用標準 <sys/ioctl.h> 巨集定義命令編號（Magic = 'S'）
#define SENSOR_IOC_MAGIC 'S'
#define SENSOR_IOC_READ_REG  _IOWR(SENSOR_IOC_MAGIC, 1, SensorRegIo)
#define SENSOR_IOC_WRITE_REG _IOW(SENSOR_IOC_MAGIC, 2, SensorRegIo)

// 模擬 Kernel Driver 收到 ioctl 時的處理函式
int simulated_kernel_ioctl(int fd, unsigned long request, void* arg) {
    if (fd < 0 || arg == nullptr) return -1;

    auto* payload = static_cast<SensorRegIo*>(arg);
    if (request == SENSOR_IOC_READ_REG) {
        // 模擬讀取 Chip ID 暫存器 0x0000 回傳 0x0989
        payload->reg_val = (payload->reg_addr == 0x0000) ? 0x0989 : 0xFFFF;
        payload->status = 0;
        return 0;
    }
    if (request == SENSOR_IOC_WRITE_REG) {
        payload->status = 0;
        return 0;
    }
    return -1;
}

void ioctl_model_demo() {
    // Output:
    // === Kernel Driver ioctl Model ===
    //   ioctl read chip_id reg(0x0) = 0x989, status = 0
    //   ioctl write exposure reg(0x202) ok
    //
    cout << "=== Kernel Driver ioctl Model ===" << endl;

    UniqueFd dev_fd(open("/dev/null", O_RDWR));

    SensorRegIo query{0x0000, 0, 0};
    int ret = simulated_kernel_ioctl(dev_fd.get(), SENSOR_IOC_READ_REG, &query);
    if (ret == 0) {
        cout << "  ioctl read chip_id reg(0x" << hex << query.reg_addr
             << ") = 0x" << query.reg_val << dec
             << ", status = " << query.status << endl;
    }

    SensorRegIo write_cmd{0x0202, 1600, 0};
    if (simulated_kernel_ioctl(dev_fd.get(), SENSOR_IOC_WRITE_REG, &write_cmd) == 0) {
        cout << "  ioctl write exposure reg(0x" << hex << write_cmd.reg_addr << dec << ") ok" << endl;
    }
    cout << endl;
}

int main() {
    unique_fd_demo();
    ioctl_model_demo();
    return 0;
}
