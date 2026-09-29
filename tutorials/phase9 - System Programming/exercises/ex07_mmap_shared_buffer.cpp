/**
 * 練習 7: 用 mmap 建立匿名共享記憶體 (MAP_SHARED | MAP_ANON)
 *
 * 使用 mmap 配置一塊跨父子行程共享的 uint32_t 陣列（4 個元素），
 * 讓子行程將每個元素乘以 10，父行程 waitpid 後直接驗證共享記憶體內容。
 */

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cstdint>
#include <iostream>
using namespace std;

int main() {
    constexpr size_t kCount = 4;
    constexpr size_t kByteSize = kCount * sizeof(uint32_t);

    void* addr = mmap(nullptr, kByteSize, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANON, -1, 0);
    assert(addr != MAP_FAILED);

    auto* shared_vals = static_cast<uint32_t*>(addr);
    for (size_t i = 0; i < kCount; ++i) {
        shared_vals[i] = static_cast<uint32_t>(i + 1);  // 1, 2, 3, 4
    }

    pid_t pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        for (size_t i = 0; i < kCount; ++i) {
            shared_vals[i] *= 10;
        }
        _exit(0);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);

    assert(shared_vals[0] == 10);
    assert(shared_vals[1] == 20);
    assert(shared_vals[2] == 30);
    assert(shared_vals[3] == 40);

    munmap(addr, kByteSize);

    cout << "ex07 passed!" << endl;
    return 0;
}
