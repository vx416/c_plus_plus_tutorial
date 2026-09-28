/**
 * Phase 9-2: POSIX File Descriptors
 *
 * file descriptor 是 OS 層級的整數 handle。
 * 一般檔案、pipe、socket 都可以用 read/write 這組模型操作。
 */

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

void check_syscall(bool ok, const string& message) {
    if (!ok) {
        throw runtime_error(message + ": " + strerror(errno));
    }
}

void write_all(int fd, string_view data) {
    const char* ptr = data.data();
    size_t remaining = data.size();

    while (remaining > 0) {
        ssize_t n = write(fd, ptr, remaining);
        check_syscall(n >= 0, "write");
        ptr += n;
        remaining -= static_cast<size_t>(n);
    }
}

string read_all(int fd) {
    string result;
    char buffer[64];

    while (true) {
        ssize_t n = read(fd, buffer, sizeof(buffer));
        check_syscall(n >= 0, "read");
        if (n == 0) break;
        result.append(buffer, static_cast<size_t>(n));
    }

    return result;
}

void fd_demo() {
    // Output:
    // === POSIX file descriptor ===
    // hello fd
    cout << "=== POSIX file descriptor ===" << endl;

    const char* path = "build/fd_demo.txt";
    int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    check_syscall(fd != -1, "open write");
    write_all(fd, "hello fd\n");
    close(fd);

    fd = open(path, O_RDONLY);
    check_syscall(fd != -1, "open read");
    cout << read_all(fd);
    close(fd);
    cout << endl;
}

int main() {
    fd_demo();
    return 0;
}
