/**
 * 練習 3: join_all
 */

#include <cassert>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

template <typename T>
vector<T> join_all(vector<future<T>>& futures) {
    vector<T> results;
    for (future<T>& future : futures) {
        results.push_back(future.get());
    }
    return results;
}

int main() {
    vector<future<int>> futures;
    vector<thread> workers;

    for (int value : {10, 20, 12}) {
        packaged_task<int()> task([value] {
            return value;
        });
        futures.push_back(task.get_future());
        workers.emplace_back(std::move(task));
    }

    assert((join_all(futures) == vector<int>{10, 20, 12}));
    for (thread& worker : workers) {
        worker.join();
    }

    cout << "ex03 passed!" << endl;
    return 0;
}
