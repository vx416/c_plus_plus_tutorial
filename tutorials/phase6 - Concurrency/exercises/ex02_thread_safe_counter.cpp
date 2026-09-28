/**
 * 練習 2: Thread-safe Counter
 *
 * 用 mutex 保護 counter，讓多個 thread 同時 increment 也得到正確結果。
 *
 * 重點練習：
 *   - lock_guard
 *   - 共享資料要用同一把 mutex 保護
 */

#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

class Counter {
public:
    void increment() {
        lock_guard<mutex> lock(mutex_);
        ++value_;
    }

    int value() const {
        lock_guard<mutex> lock(mutex_);
        return value_;
    }

private:
    mutable mutex mutex_;
    int value_ = 0;
};

int main() {
    Counter counter;
    vector<thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&] {
            for (int j = 0; j < 1000; ++j) {
                counter.increment();
            }
        });
    }

    for (thread& thread : threads) {
        thread.join();
    }

    assert(counter.value() == 4000);
    cout << "ex02 passed!" << endl;
    return 0;
}
