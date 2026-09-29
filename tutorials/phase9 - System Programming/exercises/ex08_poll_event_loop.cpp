/**
 * 練習 8: 用 poll() 找出哪個 Pipe 有資料可讀
 *
 * 給定兩個 pipe 的讀取端 fd_a 與 fd_b，只對其中一個寫入資料，
 * 請實作 read_ready_pipe(int fd_a, int fd_b) 用 poll() 偵測並讀出字串。
 */

#include <poll.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

string read_ready_pipe(int fd_a, int fd_b) {
    pollfd fds[2]{};
    fds[0].fd = fd_a;
    fds[0].events = POLLIN;
    fds[1].fd = fd_b;
    fds[1].events = POLLIN;

    int ready = poll(fds, 2, 500);
    if (ready <= 0) {
        return "";
    }

    char buf[64]{};
    for (const auto& pfd : fds) {
        if (pfd.revents & POLLIN) {
            ssize_t n = read(pfd.fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                return string(buf, static_cast<size_t>(n));
            }
        }
    }
    return "";
}

int main() {
    int pipe_a[2];
    int pipe_b[2];
    assert(pipe(pipe_a) == 0);
    assert(pipe(pipe_b) == 0);

    // 只寫進 pipe_b，如果直接 blocking read(pipe_a[0]) 就會永久卡死
    const char* payload = "vsync_tick_42";
    write(pipe_b[1], payload, strlen(payload));

    string result = read_ready_pipe(pipe_a[0], pipe_b[0]);
    assert(result == "vsync_tick_42");

    close(pipe_a[0]);
    close(pipe_a[1]);
    close(pipe_b[0]);
    close(pipe_b[1]);

    cout << "ex08 passed!" << endl;
    return 0;
}
