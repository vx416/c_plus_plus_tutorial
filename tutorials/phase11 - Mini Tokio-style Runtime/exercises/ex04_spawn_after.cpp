/**
 * 練習 4: spawn_after
 */

#include <cassert>
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
        for (size_t i = 0; i < workers; ++i) workers_.emplace_back([this] { loop(); });
    }

    ~Runtime() {
        for (thread& timer : timers_) timer.join();
        {
            lock_guard<mutex> lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();
        for (thread& worker : workers_) worker.join();
    }

    template <typename Rep, typename Period, typename F>
    auto spawn_after(chrono::duration<Rep, Period> delay, F&& f) -> future<invoke_result_t<F&>> {
        using Result = invoke_result_t<F&>;
        auto task = make_shared<packaged_task<Result()>>(std::forward<F>(f));
        future<Result> result = task->get_future();

        timers_.emplace_back([this, delay, task] {
            this_thread::sleep_for(delay);
            {
                lock_guard<mutex> lock(mutex_);
                tasks_.push([task] { (*task)(); });
            }
            cv_.notify_one();
        });

        return result;
    }

private:
    void loop() {
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
    Runtime rt(1);
    auto result = rt.spawn_after(5ms, [] { return string("done"); });
    assert(result.get() == "done");

    cout << "ex04 passed!" << endl;
    return 0;
}
