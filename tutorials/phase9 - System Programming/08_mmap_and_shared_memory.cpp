/**
 * Phase 9-8: mmap & Zero-Copy Shared Memory (DMA-BUF / Gralloc / ashmem 基礎)
 *
 * 在 Android 系統中，一張 12MP RAW16 相機影像約 24 MB，每秒 60 張就是 1.44 GB/s。
 * 如果從 Kernel -> Camera HAL -> App 每一層都用 read()/write() 或 socket 複製記憶體，
 * CPU 頻寬與耗電會瞬間爆表。
 *
 * 解法是 Zero-Copy Shared Memory：
 *   1. 配置一塊底層實體/虛擬記憶體，取得一個 File Descriptor (如 DMA-BUF / ashmem / AHardwareBuffer)。
 *   2. 跨行程只傳遞這個整數 FD（幾個 bytes）。
 *   3. 接收端呼叫 `mmap(..., MAP_SHARED, fd, 0)` 把同一塊記憶體映射到自己的虛擬位址空間直接讀寫！
 *
 * 目錄:
 *   1. mmap 基本用法與 RAII ScopedMmap 封裝
 *   2. 跨行程 (fork) MAP_SHARED 零拷貝共享記憶體示範
 *   3. Android DMA-BUF / AHardwareBuffer 零拷貝流水線心智模型
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
using namespace std;

// ============================================================
// 1. RAII ScopedMmap 封裝
// ============================================================
// 本章重點：
//   mmap 失敗時回傳的是 MAP_FAILED ((void*)-1)，不是 nullptr！
//   離開生命週期時必須呼叫 munmap(addr, size) 解除映射。
class ScopedMmap {
public:
    ScopedMmap(void* addr, size_t size) : addr_(addr), size_(size) {}

    ~ScopedMmap() {
        if (valid()) {
            munmap(addr_, size_);
        }
    }

    ScopedMmap(const ScopedMmap&) = delete;
    ScopedMmap& operator=(const ScopedMmap&) = delete;

    bool valid() const { return addr_ != nullptr && addr_ != MAP_FAILED; }
    void* data() const { return addr_; }
    size_t size() const { return size_; }

    template <typename T>
    T* as() const {
        return static_cast<T*>(addr_);
    }

private:
    void* addr_ = MAP_FAILED;
    size_t size_ = 0;
};

// ============================================================
// 2. 跨行程 MAP_SHARED 零拷貝共享記憶體
// ============================================================
struct SharedFrameBuffer {
    uint32_t frame_number;
    uint32_t width;
    uint32_t height;
    uint16_t first_pixels[4];
};

void mmap_shared_demo() {
    // Output:
    // === mmap MAP_SHARED Zero-Copy ===
    //   parent sees frame_number = 42, size = 4000x3000
    //   parent sees pixels = 100 200 300 400
    //
    cout << "=== mmap MAP_SHARED Zero-Copy ===" << endl;

    const char* shm_path = "build/shm_frame.bin";
    int fd = open(shm_path, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
        throw runtime_error("open failed");
    }

    // 先把檔案大小調整為 SharedFrameBuffer 的大小
    if (ftruncate(fd, sizeof(SharedFrameBuffer)) == -1) {
        close(fd);
        throw runtime_error("ftruncate failed");
    }

    // 映射成 MAP_SHARED：任何行程修改這塊記憶體，其他映射同一個 fd 的行程直接看得到
    ScopedMmap mapping(
        mmap(nullptr, sizeof(SharedFrameBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0),
        sizeof(SharedFrameBuffer));

    // mmap 完成後，原本的 fd 甚至可以先 close，映射依然有效
    close(fd);

    pid_t pid = fork();
    if (pid == 0) {
        // 子行程（模擬 ISP / Camera HAL Producer）直接寫入映射記憶體，不呼叫 write()
        auto* frame = mapping.as<SharedFrameBuffer>();
        frame->frame_number = 42;
        frame->width = 4000;
        frame->height = 3000;
        frame->first_pixels[0] = 100;
        frame->first_pixels[1] = 200;
        frame->first_pixels[2] = 300;
        frame->first_pixels[3] = 400;
        _exit(0);
    }

    int status = 0;
    waitpid(pid, &status, 0);

    // 父行程（模擬 Consumer）直接從同一塊映射記憶體讀取，零拷貝！
    const auto* frame = mapping.as<const SharedFrameBuffer>();
    cout << "  parent sees frame_number = " << frame->frame_number
         << ", size = " << frame->width << "x" << frame->height << endl;
    cout << "  parent sees pixels = "
         << frame->first_pixels[0] << " "
         << frame->first_pixels[1] << " "
         << frame->first_pixels[2] << " "
         << frame->first_pixels[3] << endl;
    cout << endl;
}

// ============================================================
// 3. Android Zero-Copy Buffer 架構對照
// ============================================================
void android_buffer_architecture_demo() {
    // Output:
    // === Android Zero-Copy Buffer Flow ===
    //   1. Gralloc / DMA-BUF allocates physical/ION memory -> returns buffer_fd
    //   2. Binder IPC passes buffer_fd (handle) from App/Framework to Camera HAL
    //   3. ISP DMA writes sensor frame directly into physical memory
    //   4. CPU (if needed) calls mmap(buffer_fd) to inspect metadata/pixels in-place
    //
    cout << "=== Android Zero-Copy Buffer Flow ===" << endl;
    cout << "  1. Gralloc / DMA-BUF allocates physical/ION memory -> returns buffer_fd" << endl;
    cout << "  2. Binder IPC passes buffer_fd (handle) from App/Framework to Camera HAL" << endl;
    cout << "  3. ISP DMA writes sensor frame directly into physical memory" << endl;
    cout << "  4. CPU (if needed) calls mmap(buffer_fd) to inspect metadata/pixels in-place" << endl;
    cout << endl;
}

int main() {
    mmap_shared_demo();
    android_buffer_architecture_demo();
    return 0;
}
