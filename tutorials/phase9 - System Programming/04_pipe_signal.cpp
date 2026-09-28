/**
 * Phase 9-4: Pipe & Signal
 *
 * pipe 是單向通道：fd[1] 寫入，fd[0] 讀出。
 * signal handler 裡要保持極簡，通常只設定 sig_atomic_t flag。
 */

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

volatile sig_atomic_t got_signal = 0;

void handle_sigusr1(int) {
    got_signal = 1;
}

string pipe_roundtrip() {
    int fds[2];
    if (pipe(fds) == -1) {
        throw runtime_error("pipe failed");
    }

    pid_t pid = fork();
    if (pid == -1) {
        throw runtime_error("fork failed");
    }

    if (pid == 0) {
        close(fds[0]);
        const char* msg = "hello pipe";
        write(fds[1], msg, strlen(msg));
        close(fds[1]);
        _exit(0);
    }

    close(fds[1]);
    char buffer[64] = {};
    ssize_t n = read(fds[0], buffer, sizeof(buffer));
    close(fds[0]);
    waitpid(pid, nullptr, 0);

    if (n < 0) {
        throw runtime_error("read failed");
    }
    return string(buffer, static_cast<size_t>(n));
}

int main() {
    // Output:
    // === pipe / signal ===
    //   got signal = 1
    //   pipe message = hello pipe
    cout << "=== pipe / signal ===" << endl;

    struct sigaction action {};
    action.sa_handler = handle_sigusr1;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, nullptr);

    raise(SIGUSR1);
    cout << "  got signal = " << got_signal << endl;
    cout << "  pipe message = " << pipe_roundtrip() << endl;
    cout << endl;
    return 0;
}
