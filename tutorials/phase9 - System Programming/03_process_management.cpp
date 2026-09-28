/**
 * Phase 9-3: Process Management
 *
 * fork 建立子行程。父子行程從 fork 後的下一行繼續跑，但 fork 回傳值不同：
 *   child: 0
 *   parent: child pid
 *   error: -1
 */

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
using namespace std;

int run_child_and_get_status() {
    pid_t pid = fork();
    if (pid == -1) {
        throw runtime_error(string("fork failed: ") + strerror(errno));
    }

    if (pid == 0) {
        _exit(7);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        throw runtime_error(string("waitpid failed: ") + strerror(errno));
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

int main() {
    // Output:
    // === process management ===
    //   child exit status = 7
    cout << "=== process management ===" << endl;
    cout << "  child exit status = " << run_child_and_get_status() << endl;
    cout << endl;
    return 0;
}
