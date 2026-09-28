/**
 * 練習 3: process exit status
 *
 * fork 子行程，父行程用 waitpid 取得 exit status。
 */

#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
using namespace std;

int run_child(int code) {
    pid_t pid = fork();
    assert(pid != -1);

    if (pid == 0) {
        _exit(code);
    }

    int status = 0;
    assert(waitpid(pid, &status, 0) == pid);
    assert(WIFEXITED(status));
    return WEXITSTATUS(status);
}

int main() {
    assert(run_child(3) == 3);
    assert(run_child(42) == 42);

    cout << "ex03 passed!" << endl;
    return 0;
}
