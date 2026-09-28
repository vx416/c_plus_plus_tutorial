/**
 * 練習 5: Atomic Counter
 *
 * 用 atomic<int> 做多 thread counter。
 *
 * 重點練習：
 *   - atomic increment
 *   - memory_order_relaxed 適合獨立統計計數
 */

#include <cassert>
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

int main() {
    atomic<int> counter{0};
    vector<thread> workers;

    for (int i = 0; i < 4; ++i) {
        workers.emplace_back([&] {
            for (int j = 0; j < 1000; ++j) {
                counter.fetch_add(1, memory_order_relaxed);
            }
        });
    }

    for (thread& worker : workers) {
        worker.join();
    }

    assert(counter.load(memory_order_relaxed) == 4000);
    cout << "ex05 passed!" << endl;
    return 0;
}
