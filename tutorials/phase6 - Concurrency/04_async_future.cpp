/**
 * Phase 6-4: std::async & std::future
 *
 * async 是 task-based concurrency：你交出一個工作，future 代表之後會拿到結果。
 *
 * 目錄:
 *   1. async 建立 task
 *   2. future 背後是誰執行
 *   3. promise 和 packaged_task
 *   4. future::get
 *   5. exception 傳回呼叫端
 *   6. launch policy
 *   7. std::async 不能指定 thread pool
 *   8. async/future 不是 epoll runtime
 *   9. 最小 ThreadPool + future
 */

#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>
using namespace std;
using namespace std::chrono_literals;

int slow_square(int value) {
    this_thread::sleep_for(5ms);
    return value * value;
}

// ============================================================
// 1. async 建立 task
// ============================================================
// 本章重點：
//   std::async 回傳 future。
//   future 不是結果本身，而是「未來可以取得結果的 handle」。
void async_demo() {
    // Output:
    // === async ===
    //   result = 25
    //
    cout << "=== async ===" << endl;

    future<int> result = async(launch::async, slow_square, 5);
    cout << "  result = " << result.get() << endl;
    cout << endl;
}

// ============================================================
// 2. future 背後是誰執行
// ============================================================
// 本章重點：
//   future 本身不執行工作，它只是「之後拿結果」的 handle。
//   背後誰在跑，取決於 future 是怎麼被建立的。
//
//   常見來源：
//     async(launch::async, f)     標準庫安排 f 非同步執行，通常是另一條 thread。
//     async(launch::deferred, f)  不開背景 thread，等 get/wait 時在呼叫端執行。
//     promise                    由你自己決定哪個 thread 呼叫 set_value/set_exception。
//     packaged_task              把 callable 接到 future，誰執行 task 由你決定。
//     ThreadPool::submit          worker thread 執行 packaged_task，future 拿結果。
//
//   注意：
//     std::future 不是 executor，也不是 thread pool handle。
//     std::async 沒保證使用 system thread pool；標準只規定語意，不指定底層實作。
void future_execution_owner_demo() {
    // Output:
    // === who runs a future task ===
    //   future only stores a result channel; it does not run work by itself
    //   async/promise/packaged_task/thread pool decide who executes the work
    //
    cout << "=== who runs a future task ===" << endl;
    cout << "  future only stores a result channel; it does not run work by itself" << endl;
    cout << "  async/promise/packaged_task/thread pool decide who executes the work" << endl;
    cout << endl;
}

// ============================================================
// 3. promise 和 packaged_task
// ============================================================
// 本章重點：
//   promise<T> 和 future<T> 連到同一個 shared state。
//     promise 負責 set_value/set_exception。
//     future 負責 get/wait。
//
//   packaged_task<R()> 是「callable + future shared state」的包裝。
//   你可以先拿到 future，再把 packaged_task 丟給指定的 thread 或 thread pool 執行。
//
//   差別：
//     promise        適合手動完成結果，例如 callback 或事件完成時 set_value。
//     packaged_task  適合包一個會 return 的 callable，執行後自動完成 future。
void promise_demo() {
    // Output:
    // === promise ===
    //   promise result = 36
    //
    cout << "=== promise ===" << endl;

    promise<int> result_promise;
    future<int> result = result_promise.get_future();

    thread worker([p = std::move(result_promise)]() mutable {
        p.set_value(slow_square(6));
    });

    cout << "  promise result = " << result.get() << endl;
    worker.join();
    cout << endl;
}

void packaged_task_demo() {
    // Output:
    // === packaged_task ===
    //   packaged_task result = 49
    //
    cout << "=== packaged_task ===" << endl;

    packaged_task<int()> task([] {
        return slow_square(7);
    });
    future<int> result = task.get_future();

    thread worker(std::move(task));

    cout << "  packaged_task result = " << result.get() << endl;
    worker.join();
    cout << endl;
}

// ============================================================
// 4. future::get
// ============================================================
// 本章重點：
//   get 會等待 task 完成並取回結果。
//   同一個 future 的 get 只能呼叫一次，因為結果會被取走。
void get_demo() {
    // Output:
    // === future::get ===
    //   value = 42
    //
    cout << "=== future::get ===" << endl;

    auto task = async(launch::async, [] { return 42; });
    int value = task.get();
    cout << "  value = " << value << endl;
    cout << endl;
}

// ============================================================
// 5. exception 傳回呼叫端
// ============================================================
// 本章重點：
//   async task 裡丟出的 exception 會存進 future。
//   呼叫 get 時，exception 會在呼叫端重新丟出。
void exception_demo() {
    // Output:
    // === exception propagation ===
    //   caught: task failed
    //
    cout << "=== exception propagation ===" << endl;

    auto task = async(launch::async, []() -> int {
        throw runtime_error("task failed");
    });

    try {
        (void)task.get();
    } catch (const runtime_error& ex) {
        cout << "  caught: " << ex.what() << endl;
    }
    cout << endl;
}

// ============================================================
// 6. launch policy
// ============================================================
// 本章重點：
//   launch::async 表示另開 thread 執行。
//   launch::deferred 表示等 get/wait 時才在呼叫端執行。
//   沒指定 policy 時，標準庫可以自己選，所以教學範例常明確寫 launch::async。
//
//   注意：
//     launch::async 不代表「使用 thread pool」。
//     標準只規定它要非同步執行，不規定底層是新 thread、內部 pool、或其他機制。
void launch_policy_demo() {
    // Output:
    // === launch policy ===
    //   launch::async: run on another thread
    //   launch::deferred: run when get/wait is called
    //
    cout << "=== launch policy ===" << endl;
    cout << "  launch::async: run on another thread" << endl;
    cout << "  launch::deferred: run when get/wait is called" << endl;
    cout << endl;
}

// ============================================================
// 7. std::async 不能指定 thread pool
// ============================================================
// 本章重點：
//   標準 C++ 沒有 std::async(pool, task) 這種 API。
//   std::future 也只是結果 handle，不知道 task 在哪個 executor 或 pool 跑。
//
//   如果你需要：
//     - 固定 N 個 worker
//     - 限制同時執行數量
//     - task queue
//     - 明確指定任務跑在哪個 pool
//
//   就要自己寫 thread pool，或用 Boost.Asio / oneTBB / folly 等 library。
void async_pool_limit_demo() {
    // Output:
    // === std::async cannot choose a thread pool ===
    //   std::async only accepts launch policy, not a user-provided pool
    //   use a ThreadPool::submit(...) API when pool ownership matters
    //
    cout << "=== std::async cannot choose a thread pool ===" << endl;
    cout << "  std::async only accepts launch policy, not a user-provided pool" << endl;
    cout << "  use a ThreadPool::submit(...) API when pool ownership matters" << endl;
    cout << endl;
}

// ============================================================
// 8. async/future 不是 epoll runtime
// ============================================================
// 本章重點：
//   std::async/std::future 是 task/result abstraction。
//   它不是 Tokio / Boost.Asio 那種 async I/O runtime。
//
//   它不提供：
//     - epoll / kqueue / IOCP event loop
//     - non-blocking socket readiness
//     - timer reactor
//     - coroutine executor
//
//   要做大量 network I/O，通常看 Boost.Asio、libuv、folly、或自己寫 OS-specific event loop。
void async_not_io_runtime_demo() {
    // Output:
    // === async/future is not an I/O runtime ===
    //   std::async runs callables and returns futures
    //   epoll/kqueue/IOCP based I/O needs a separate runtime/library
    //
    cout << "=== async/future is not an I/O runtime ===" << endl;
    cout << "  std::async runs callables and returns futures" << endl;
    cout << "  epoll/kqueue/IOCP based I/O needs a separate runtime/library" << endl;
    cout << endl;
}

// ============================================================
// 9. 最小 ThreadPool + future
// ============================================================
// 本章重點：
//   ThreadPool 的基本結構：
//     固定數量 worker threads
//     queue<function<void()>>
//     mutex + condition_variable
//     packaged_task 把 task 的結果接到 future
//
//   packaged_task<R()> 是「會產生 future<R> 的 callable 包裝」。
//   worker 執行 packaged_task 後，future.get() 就能拿到結果或 exception。
class ThreadPool {
public:
    explicit ThreadPool(size_t worker_count) {
        for (size_t i = 0; i < worker_count; ++i) {
            workers_.emplace_back([this] {
                worker_loop();
            });
        }
    }

    ~ThreadPool() {
        {
            lock_guard<mutex> lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();

        for (thread& worker : workers_) {
            worker.join();
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename F>
    auto submit(F&& f) -> future<invoke_result_t<F&>> {
        using Result = invoke_result_t<F&>;

        auto task = make_shared<packaged_task<Result()>>(std::forward<F>(f));
        future<Result> result = task->get_future();

        {
            lock_guard<mutex> lock(mutex_);
            tasks_.push([task] {
                (*task)();
            });
        }
        cv_.notify_one();

        return result;
    }

private:
    void worker_loop() {
        while (true) {
            function<void()> task;

            {
                unique_lock<mutex> lock(mutex_);
                cv_.wait(lock, [this] {
                    return stopping_ || !tasks_.empty();
                });

                if (stopping_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            task();
        }
    }

    vector<thread> workers_;
    queue<function<void()>> tasks_;
    mutex mutex_;
    condition_variable cv_;
    bool stopping_ = false;
};

void thread_pool_demo() {
    // Output:
    // === minimal ThreadPool + future ===
    //   total squares = 14
    cout << "=== minimal ThreadPool + future ===" << endl;

    ThreadPool pool(2);
    vector<future<int>> tasks;
    for (int value : {1, 2, 3}) {
        tasks.push_back(pool.submit([value] {
            return slow_square(value);
        }));
    }

    int total = 0;
    for (future<int>& task : tasks) {
        total += task.get();
    }
    cout << "  total squares = " << total << endl;
    cout << endl;
}

int main() {
    async_demo();
    future_execution_owner_demo();
    promise_demo();
    packaged_task_demo();
    get_demo();
    exception_demo();
    launch_policy_demo();
    async_pool_limit_demo();
    async_not_io_runtime_demo();
    thread_pool_demo();
    return 0;
}
