/**
 * Phase 6-5: Atomic & Memory Order
 *
 * atomic 讓單一變數的操作不可分割，避免 data race。
 * memory_order 用來描述 atomic 操作之間的可見性規則。
 *
 * 目錄:
 *   1. atomic counter
 *   2. fetch_add
 *   3. relaxed ordering
 *   4. release/acquire
 *   5. 實務準則
 */

#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

// ============================================================
// 1. atomic counter
// ============================================================
// 本章重點：
//   atomic<int> 的 ++ 是 atomic operation。
//   多個 thread 同時 increment 不會造成 data race。
void atomic_counter_demo() {
    // Output:
    // === atomic counter ===
    //   counter = 4000
    //
    cout << "=== atomic counter ===" << endl;

    atomic<int> counter{0};
    vector<thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back([&] {
            for (int j = 0; j < 1000; ++j) {
                ++counter;
            }
        });
    }
    for (thread& worker : workers) {
        worker.join();
    }
    cout << "  counter = " << counter.load() << endl << endl;
}

// ============================================================
// 2. fetch_add
// ============================================================
// 本章重點：
//   fetch_add 會回傳加之前的舊值。
//   它常用來發號碼牌、計數、分配 index。
void fetch_add_demo() {
    // Output:
    // === fetch_add ===
    //   old id = 100, next = 101
    //
    cout << "=== fetch_add ===" << endl;

    atomic<int> next_id{100};
    int old = next_id.fetch_add(1);
    cout << "  old id = " << old << ", next = " << next_id.load() << endl;
    cout << endl;
}

// ============================================================
// 3. relaxed ordering
// ============================================================
// 本章重點：
//   memory_order_relaxed 只保證該 atomic 變數本身操作不被撕裂。
//   它不保證和其他資料的可見順序。
//   適合純計數、統計，不拿它同步其他資料。
void relaxed_demo() {
    // Output:
    // === memory_order_relaxed ===
    //   hits = 1
    //
    cout << "=== memory_order_relaxed ===" << endl;

    atomic<int> hits{0};
    hits.fetch_add(1, memory_order_relaxed);
    cout << "  hits = " << hits.load(memory_order_relaxed) << endl;
    cout << endl;
}

// ============================================================
// 4. release/acquire
// ============================================================
// 本章重點：
//   release/acquire 常用來發布資料：
//     producer 先寫資料，再用 release store 設 ready=true
//     consumer 用 acquire load 看到 ready=true 後，可以看見 producer 先前寫的資料
//
//   這是同步「atomic flag 以外的資料」時的重要模式。
void release_acquire_demo() {
    // Output:
    // === release / acquire ===
    //   message = published
    //
    cout << "=== release / acquire ===" << endl;

    string message;
    atomic<bool> ready{false};

    thread producer([&] {
        message = "published";
        ready.store(true, memory_order_release);
    });

    thread consumer([&] {
        while (!ready.load(memory_order_acquire)) {
        }
        cout << "  message = " << message << endl;
    });

    producer.join();
    consumer.join();
    cout << endl;
}

// ============================================================
// 5. 實務準則
// ============================================================
// 本章重點：
//   先用 mutex 解決共享狀態，確定需要 lock-free 再考慮 atomic。
//   atomic 適合單一變數或很小的同步協定。
//   不懂 memory_order 時，不要亂用 relaxed；預設 sequential consistency 比較安全。
void guideline_demo() {
    // Output:
    // === atomic guidelines ===
    //   use mutex first for compound state
    //   use relaxed only for independent counters
    //   use release/acquire to publish data through a flag
    cout << "=== atomic guidelines ===" << endl;
    cout << "  use mutex first for compound state" << endl;
    cout << "  use relaxed only for independent counters" << endl;
    cout << "  use release/acquire to publish data through a flag" << endl;
    cout << endl;
}

int main() {
    atomic_counter_demo();
    fetch_add_demo();
    relaxed_demo();
    release_acquire_demo();
    guideline_demo();
    return 0;
}
