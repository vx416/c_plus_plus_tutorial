/**
 * 練習 3: Blocking Queue
 *
 * 實作一個簡單 blocking queue，producer push，consumer pop。
 *
 * 重點練習：
 *   - condition_variable
 *   - wait predicate
 *   - close 讓 consumer 可以結束
 */

#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
using namespace std;

class BlockingQueue {
public:
    void push(int value) {
        {
            lock_guard<mutex> lock(mutex_);
            values_.push(value);
        }
        cv_.notify_one();
    }

    optional<int> pop() {
        unique_lock<mutex> lock(mutex_);
        cv_.wait(lock, [&] {
            return closed_ || !values_.empty();
        });

        if (values_.empty()) {
            return nullopt;
        }

        int value = values_.front();
        values_.pop();
        return value;
    }

    void close() {
        {
            lock_guard<mutex> lock(mutex_);
            closed_ = true;
        }
        cv_.notify_all();
    }

private:
    mutex mutex_;
    condition_variable cv_;
    queue<int> values_;
    bool closed_ = false;
};

int main() {
    BlockingQueue queue;
    int sum = 0;

    thread consumer([&] {
        while (optional<int> value = queue.pop()) {
            sum += *value;
        }
    });

    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.close();
    consumer.join();

    assert(sum == 6);
    cout << "ex03 passed!" << endl;
    return 0;
}
