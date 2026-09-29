/**
 * Phase 9-9: I/O Multiplexing with poll() (Event Loop 基礎)
 *
 * 當一個執行緒同時需要等待多個事件來源時：
 *   - fd 0: 硬體 VSync / SOF (Start-of-Frame) 中斷通知
 *   - fd 1: 上層傳來的停止 (Shutdown / Flush) 控制管線
 *
 * 如果直接對 fd 0 呼叫阻塞式 `read(fd0, ...)`，那當 fd 1 送來 Shutdown 時，
 * 執行緒會卡死在 fd 0 無法醒來！
 *
 * 解法是 I/O Multiplexing（多工）：
 *   用 `poll()`（Linux 上大量 fd 時進階用 `epoll`，Android 封裝為 `android::Looper`）
 *   同時監聽一組 `pollfd`，哪個 fd 準備好 (POLLIN) 就處理哪個。
 */

#include <poll.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void poll_event_loop_demo() {
    // Output:
    // === poll() Event Loop ===
    //   [EventLoop] sensor pipe ready -> msg: SOF_FRAME_1
    //   [EventLoop] control pipe ready -> msg: STOP
    //   [EventLoop] shutdown requested, exiting cleanly
    //
    cout << "=== poll() Event Loop ===" << endl;

    int sensor_pipe[2];
    int control_pipe[2];
    pipe(sensor_pipe);
    pipe(control_pipe);

    // 預先寫入一筆感測器事件與一筆控制指令
    const char* sof_msg = "SOF_FRAME_1";
    write(sensor_pipe[1], sof_msg, strlen(sof_msg));

    const char* stop_msg = "STOP";
    write(control_pipe[1], stop_msg, strlen(stop_msg));

    // 準備 pollfd 陣列，同時監聽兩個讀取端
    pollfd fds[2]{};
    fds[0].fd = sensor_pipe[0];
    fds[0].events = POLLIN;
    fds[1].fd = control_pipe[0];
    fds[1].events = POLLIN;

    bool running = true;
    while (running) {
        // timeout_ms = 1000：最多等 1 秒，有任一 fd 可讀就立刻返回
        int ready = poll(fds, 2, 1000);
        if (ready <= 0) {
            break;
        }

        if (fds[0].revents & POLLIN) {
            char buf[64]{};
            ssize_t n = read(fds[0].fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                cout << "  [EventLoop] sensor pipe ready -> msg: " << buf << endl;
            }
        }

        if (fds[1].revents & POLLIN) {
            char buf[64]{};
            ssize_t n = read(fds[1].fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                cout << "  [EventLoop] control pipe ready -> msg: " << buf << endl;
                if (string(buf) == "STOP") {
                    cout << "  [EventLoop] shutdown requested, exiting cleanly" << endl;
                    running = false;
                }
            }
        }
    }

    close(sensor_pipe[0]);
    close(sensor_pipe[1]);
    close(control_pipe[0]);
    close(control_pipe[1]);
    cout << endl;
}

int main() {
    poll_event_loop_demo();
    return 0;
}
