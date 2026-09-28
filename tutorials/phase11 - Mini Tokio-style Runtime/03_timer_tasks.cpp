/**
 * Phase 11-3: Timer Tasks
 *
 * spawn_after 不是完整 reactor，只是示範「延遲後把 packaged_task 排進 runtime」。
 */

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>
using namespace std;
using namespace std::chrono_literals;

class Runtime {
public:
    explicit Runtime(size_t workers) {
        for (size_t i = 0; i < workers; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~Runtime() {
        for (thread& timer : timers_) {
            timer.join();
        }

        {
            lock_guard<mutex> lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();

        for (thread& worker : workers_) {
            worker.join();
        }
    }

    template <typename F>
    auto spawn(F&& f) -> future<invoke_result_t<F&>> {
        using Result = invoke_result_t<F&>;
        auto task = make_shared<packaged_task<Result()>>(std::forward<F>(f));
        future<Result> result = task->get_future();
        enqueue([task] { (*task)(); });
        return result;
    }

    template <typename Rep, typename Period, typename F>
    auto spawn_after(chrono::duration<Rep, Period> delay, F&& f) -> future<invoke_result_t<F&>> {
        using Result = invoke_result_t<F&>;
        auto task = make_shared<packaged_task<Result()>>(std::forward<F>(f));
        future<Result> result = task->get_future();

        timers_.emplace_back([this, delay, task] {
            this_thread::sleep_for(delay);
            enqueue([task] { (*task)(); });
        });

        return result;
    }

private:
    void enqueue(function<void()> task) {
        {
            lock_guard<mutex> lock(mutex_);
            tasks_.push(std::move(task));
        }
        cv_.notify_one();
    }

    void worker_loop() {
        while (true) {
            function<void()> task;
            {
                unique_lock<mutex> lock(mutex_);
                cv_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });
                if (stopping_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    vector<thread> workers_;
    vector<thread> timers_;
    queue<function<void()>> tasks_;
    mutex mutex_;
    condition_variable cv_;
    bool stopping_ = false;
};

int main() {
    // Output:
    // === timer tasks ===
    //   timer fired
    cout << "=== timer tasks ===" << endl;

    Runtime rt(1);
    auto future = rt.spawn_after(10ms, [] {
        return string("timer fired");
    });

    cout << "  " << future.get() << endl << endl;
    return 0;
}
