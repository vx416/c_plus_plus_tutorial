/**
 * Phase 6-2: Mutex & Lock
 *
 * mutex 用來保護共享資料，避免多個 thread 同時修改造成 data race。
 *
 * 目錄:
 *   1. data race 問題
 *   2. lock_guard
 *   3. unique_lock
 *   4. scoped_lock 多把鎖
 *   5. 降低 lock 範圍
 */

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

// ============================================================
// 1. data race 問題
// ============================================================
// 本章重點：
//   多個 thread 同時讀寫同一個變數，且至少一個是寫，就是 data race。
//   data race 是 undefined behavior，不是「結果可能差一點」而已。
void data_race_demo() {
    // Output:
    // === data race ===
    //   shared mutable data must be synchronized
    //
    cout << "=== data race ===" << endl;
    cout << "  shared mutable data must be synchronized" << endl;
    cout << endl;
}

// ============================================================
// 2. lock_guard
// ============================================================
// 本章重點：
//   lock_guard 是 RAII lock。
//   建構時 lock mutex，離開 scope 自動 unlock。
//   它適合簡單區塊：進入就鎖，離開就解。
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

void lock_guard_demo() {
    // Output:
    // === lock_guard ===
    //   counter = 4000
    //
    cout << "=== lock_guard ===" << endl;

    Counter counter;
    vector<thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back([&] {
            for (int j = 0; j < 1000; ++j) {
                counter.increment();
            }
        });
    }
    for (thread& worker : workers) {
        worker.join();
    }
    cout << "  counter = " << counter.value() << endl << endl;
}

// ============================================================
// 3. unique_lock
// ============================================================
// 本章重點：
//   unique_lock 比 lock_guard 彈性高。
//   它可以延後 lock、提前 unlock，也能搭配 condition_variable。
//   代價是稍微重一點；簡單情況仍優先 lock_guard。
void unique_lock_demo() {
    // Output:
    // === unique_lock ===
    //   locked
    //   unlocked early
    //
    cout << "=== unique_lock ===" << endl;

    mutex m;
    unique_lock<mutex> lock(m);
    cout << "  locked" << endl;
    lock.unlock();
    cout << "  unlocked early" << endl << endl;
}

// ============================================================
// 4. scoped_lock 多把鎖
// ============================================================
// 本章重點：
//   同時鎖多把 mutex 時，自己手寫 lock 順序容易 deadlock。
//   scoped_lock 可以一次鎖多把 mutex，標準庫會用避免 deadlock 的方式處理。
void scoped_lock_demo() {
    // Output:
    // === scoped_lock ===
    //   locked two mutexes safely
    //
    cout << "=== scoped_lock ===" << endl;

    mutex a;
    mutex b;
    {
        scoped_lock lock(a, b);
        cout << "  locked two mutexes safely" << endl;
    }
    cout << endl;
}

// ============================================================
// 5. 降低 lock 範圍
// ============================================================
// 本章重點：
//   lock 只保護共享資料，不要把慢操作也包進去。
//   鎖越久，其他 thread 等越久。
void lock_scope_demo() {
    // Output:
    // === lock scope ===
    //   keep critical sections small
    //   do slow I/O outside the lock when possible
    cout << "=== lock scope ===" << endl;
    cout << "  keep critical sections small" << endl;
    cout << "  do slow I/O outside the lock when possible" << endl;
    cout << endl;
}

int main() {
    data_race_demo();
    lock_guard_demo();
    unique_lock_demo();
    scoped_lock_demo();
    lock_scope_demo();
    return 0;
}
