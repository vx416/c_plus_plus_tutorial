/**
 * Phase 6-3: Condition Variable
 *
 * condition_variable 用來讓 thread 睡著等待某個條件成立。
 * 它通常搭配 mutex 和 queue，形成 producer-consumer pattern。
 *
 * 目錄:
 *   1. 為什麼不用 busy waiting
 *   2. wait with predicate
 *   3. notify_one
 *   4. producer-consumer queue
 *   5. shutdown 訊號
 */

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>
using namespace std;

// ============================================================
// 1. 為什麼不用 busy waiting
// ============================================================
// 本章重點：
//   busy waiting 是一直 while 檢查條件，會浪費 CPU。
//   condition_variable 可以讓 thread 睡著，等條件可能成立時再醒來檢查。
void busy_waiting_demo() {
    // Output:
    // === busy waiting ===
    //   condition_variable sleeps until notified
    //
    cout << "=== busy waiting ===" << endl;
    cout << "  condition_variable sleeps until notified" << endl;
    cout << endl;
}

// ============================================================
// 2. wait with predicate
// ============================================================
// 本章重點：
//   wait 一定要搭配 predicate。
//   因為 thread 可能 spurious wakeup，也就是沒真正滿足條件卻醒來。
//   cv.wait(lock, predicate) 會醒來後重新檢查 predicate。
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

// ============================================================
// 3. notify_one
// ============================================================
// 本章重點：
//   notify_one 叫醒一個等待中的 thread。
//   notify_all 叫醒所有等待中的 thread。
//   如果只新增一筆工作，通常 notify_one 就夠。
void notify_demo() {
    // Output:
    // === notify ===
    //   push one item -> notify_one
    //   shutdown all workers -> notify_all
    //
    cout << "=== notify ===" << endl;
    cout << "  push one item -> notify_one" << endl;
    cout << "  shutdown all workers -> notify_all" << endl;
    cout << endl;
}

// ============================================================
// 4. producer-consumer queue
// ============================================================
// 本章重點：
//   producer 負責 push 工作。
//   consumer 負責 pop 工作。
//   queue、closed flag 都是共享狀態，所以要由同一把 mutex 保護。
void producer_consumer_demo() {
    // Output:
    // === producer-consumer ===
    //   consumed sum = 10
    //
    cout << "=== producer-consumer ===" << endl;

    BlockingQueue queue;
    int sum = 0;

    thread consumer([&] {
        while (optional<int> value = queue.pop()) {
            sum += *value;
        }
    });

    for (int value : {1, 2, 3, 4}) {
        queue.push(value);
    }
    queue.close();
    consumer.join();

    cout << "  consumed sum = " << sum << endl << endl;
}

// ============================================================
// 5. shutdown 訊號
// ============================================================
// 本章重點：
//   blocking queue 需要結束訊號。
//   如果 producer 不再 push，consumer 還在 wait，就會永遠睡著。
//   close() 的目的就是讓等待中的 consumer 知道可以結束了。
void shutdown_demo() {
    // Output:
    // === shutdown ===
    //   close flag lets waiting consumers exit
    cout << "=== shutdown ===" << endl;
    cout << "  close flag lets waiting consumers exit" << endl;
    cout << endl;
}

int main() {
    busy_waiting_demo();
    notify_demo();
    producer_consumer_demo();
    shutdown_demo();
    return 0;
}
