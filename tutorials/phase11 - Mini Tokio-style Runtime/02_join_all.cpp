/**
 * Phase 11-2: join_all
 *
 * join_all 等一批 future 全部完成，並收集結果。
 */

#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

template <typename T>
vector<T> join_all(vector<future<T>>& futures) {
    vector<T> results;
    results.reserve(futures.size());

    for (future<T>& f : futures) {
        results.push_back(f.get());
    }

    return results;
}

int main() {
    // Output:
    // === join_all ===
    //   sum = 6
    cout << "=== join_all ===" << endl;

    vector<future<int>> futures;
    vector<thread> workers;

    for (int value : {1, 2, 3}) {
        packaged_task<int()> task([value] {
            return value;
        });
        futures.push_back(task.get_future());
        workers.emplace_back(std::move(task));
    }

    vector<int> values = join_all(futures);
    for (thread& worker : workers) {
        worker.join();
    }

    int sum = 0;
    for (int value : values) sum += value;

    cout << "  sum = " << sum << endl << endl;
    return 0;
}
