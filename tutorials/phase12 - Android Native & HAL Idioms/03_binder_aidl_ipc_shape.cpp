/**
 * Phase 12-3: Android Binder / AIDL IPC Shape & Cross-Process FD Passing (SCM_RIGHTS)
 *
 * 在 Android OS 中，App、CameraServer (Framework) 與 Camera HAL 跑在不同的獨立 Process：
 *   [Camera App] <--Binder--> [cameraserver] <--Binder (AIDL)--> [android.hardware.camera.provider]
 *
 * 這章用純 C++ 與 POSIX UNIX Domain Socket 拆解 Binder / AIDL 的兩個核心機制：
 *   1. AIDL Proxy (`Bp*`) 與 Stub (`Bn*`) 的分層設計：
 *      - `ICameraDevice`：純虛擬介面（Client 與 Server 共同遵守的合約）
 *      - `BpCameraDevice` (Binder Proxy)：Client 端拿到的代理物件，把參數打包進 Parcel 送過 IPC
 *      - `BnCameraDevice` (Binder Native Stub)：Server 端解開 Parcel，呼叫真正的 HAL 實作
 *   2. 跨行程傳遞 File Descriptor (`ParcelFileDescriptor` / `AHardwareBuffer` 底層原理)：
 *      為什麼 Process A 的 `fd = 5` 不能直接當整數傳給 Process B？
 *      因為每個 Process 有自己獨立的 FD Table！
 *      透過 Kernel 機制（Binder driver 或 UNIX Domain Socket 的 `SCM_RIGHTS`），
 *      Kernel 會在 Process B 的 FD Table 建立一個指向同一個底層 File/Buffer 物件的新 FD！
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. AIDL Interface / Proxy (Bp*) / Stub (Bn*) 架構模擬
// ============================================================
struct Parcel {
    int32_t command_code = 0;
    int32_t arg_value = 0;
    int32_t reply_status = 0;
    int32_t reply_value = 0;
};

// AIDL 產生的共用抽象介面
class ISensorHal {
public:
    enum TransactionCode : int32_t {
        TRANSACTION_GET_TEMPERATURE = 1,
        TRANSACTION_SET_EXPOSURE_US = 2,
    };

    virtual ~ISensorHal() = default;
    virtual int32_t GetTemperatureCelsius(int32_t* out_temp) = 0;
    virtual int32_t SetExposureUs(int32_t exposure_us) = 0;
};

// Bn* (Binder Native Stub)：跑在 HAL Server Process，負責拆開 Parcel 呼叫實作
class BnSensorHal : public ISensorHal {
public:
    void OnTransact(Parcel& parcel) {
        switch (parcel.command_code) {
            case TRANSACTION_GET_TEMPERATURE: {
                int32_t temp = 0;
                parcel.reply_status = GetTemperatureCelsius(&temp);
                parcel.reply_value = temp;
                break;
            }
            case TRANSACTION_SET_EXPOSURE_US: {
                parcel.reply_status = SetExposureUs(parcel.arg_value);
                break;
            }
            default:
                parcel.reply_status = -1;
                break;
        }
    }
};

// 真正的 Vendor HAL 實作（繼承自 BnSensorHal）
class LyricSensorHalImpl : public BnSensorHal {
public:
    int32_t GetTemperatureCelsius(int32_t* out_temp) override {
        *out_temp = 38;
        return 0;  // STATUS_OK
    }

    int32_t SetExposureUs(int32_t exposure_us) override {
        if (exposure_us <= 0) return -22;  // -EINVAL
        exposure_us_ = exposure_us;
        return 0;
    }

private:
    int32_t exposure_us_ = 10000;
};

// Bp* (Binder Proxy)：跑在 Framework Client Process，看起來像本地呼叫，實則打包 Parcel
class BpSensorHal : public ISensorHal {
public:
    explicit BpSensorHal(BnSensorHal* remote_stub) : remote_stub_(remote_stub) {}

    int32_t GetTemperatureCelsius(int32_t* out_temp) override {
        Parcel p{};
        p.command_code = TRANSACTION_GET_TEMPERATURE;
        remote_stub_->OnTransact(p);
        if (p.reply_status == 0 && out_temp != nullptr) {
            *out_temp = p.reply_value;
        }
        return p.reply_status;
    }

    int32_t SetExposureUs(int32_t exposure_us) override {
        Parcel p{};
        p.command_code = TRANSACTION_SET_EXPOSURE_US;
        p.arg_value = exposure_us;
        remote_stub_->OnTransact(p);
        return p.reply_status;
    }

private:
    BnSensorHal* remote_stub_;
};

void binder_proxy_stub_demo() {
    // Output:
    // === AIDL BpProxy / BnStub Shape ===
    //   Client calls BpSensorHal::GetTemperatureCelsius -> 38 C (status=0)
    //   Client calls BpSensorHal::SetExposureUs(16666)  -> status=0
    //
    cout << "=== AIDL BpProxy / BnStub Shape ===" << endl;

    LyricSensorHalImpl server_impl;
    BpSensorHal client_proxy(&server_impl);

    int32_t temp = 0;
    int32_t status = client_proxy.GetTemperatureCelsius(&temp);
    cout << "  Client calls BpSensorHal::GetTemperatureCelsius -> "
         << temp << " C (status=" << status << ")" << endl;

    status = client_proxy.SetExposureUs(16666);
    cout << "  Client calls BpSensorHal::SetExposureUs(16666)  -> status="
         << status << endl;
    cout << endl;
}

// ============================================================
// 2. 跨行程傳遞 File Descriptor (SCM_RIGHTS)
// ============================================================
// 透過 UNIX Domain Socket 的 ancillary data (SCM_RIGHTS) 把一個開啟的 FD 傳給另一個 Process
bool send_fd(int sock, int fd_to_send) {
    char dummy = 'F';
    iovec iov{&dummy, sizeof(dummy)};

    alignas( cmsghdr ) char cmsg_buf[CMSG_SPACE(sizeof(int))]{};
    msghdr msg{};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);

    cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    memcpy(CMSG_DATA(cmsg), &fd_to_send, sizeof(int));

    return sendmsg(sock, &msg, 0) >= 0;
}

int recv_fd(int sock) {
    char dummy = 0;
    iovec iov{&dummy, sizeof(dummy)};

    alignas( cmsghdr ) char cmsg_buf[CMSG_SPACE(sizeof(int))]{};
    msghdr msg{};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);

    if (recvmsg(sock, &msg, 0) <= 0) {
        return -1;
    }

    cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg && cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
        int received_fd = -1;
        memcpy(&received_fd, CMSG_DATA(cmsg), sizeof(int));
        return received_fd;
    }
    return -1;
}

void cross_process_fd_passing_demo() {
    // Output:
    // === Cross-Process FD Passing (SCM_RIGHTS) ===
    //   Parent received buffer_fd from HAL child -> content: RAW10_FRAME_DATA_OK
    //
    cout << "=== Cross-Process FD Passing (SCM_RIGHTS) ===" << endl;

    int socks[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) != 0) {
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // 子行程（模擬 Camera HAL）：在自己的行程空間建立檔案/Buffer 並寫入資料，再把 FD 傳給父行程
        close(socks[0]);
        int buf_fd = open("build/ipc_buffer.bin", O_CREAT | O_TRUNC | O_RDWR, 0600);
        const char* text = "RAW10_FRAME_DATA_OK";
        write(buf_fd, text, strlen(text));
        lseek(buf_fd, 0, SEEK_SET);

        send_fd(socks[1], buf_fd);
        close(buf_fd);
        close(socks[1]);
        _exit(0);
    }

    close(socks[1]);
    int remote_fd = recv_fd(socks[0]);
    close(socks[0]);

    int status = 0;
    waitpid(pid, &status, 0);

    if (remote_fd >= 0) {
        char buf[64]{};
        ssize_t n = read(remote_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            cout << "  Parent received buffer_fd from HAL child -> content: " << buf << endl;
        }
        close(remote_fd);
    }
    cout << endl;
}

int main() {
    binder_proxy_stub_demo();
    cross_process_fd_passing_demo();
    return 0;
}
