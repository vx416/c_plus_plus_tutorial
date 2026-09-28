/**
 * Phase 11-1: Runtime::spawn
 *
 * 用 packaged_task 實作 spawn(callable) -> future<R>。
 */

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

class Runtime {
public:
    explicit Runtime(size_t workers) {
        for (size_t i = 0; i < workers; ++i) {
            workers_.emplace_back([this] {
                worker_loop();
            });
        }
    }

    ~Runtime() {
        {
            lock_guard<mutex> lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();

        for (thread& worker : workers_) {
            worker.join();
        }
    }

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;

    template <typename F>
    auto spawn(F&& f) -> future<invoke_result_t<F&>> {
        // F&& 是 forwarding reference：lambda 傳 lvalue 就 copy 進 task，傳 rvalue 就 move 進去。
        // 原理見 phase5 01_move_semantics 第 7 章，坑見 phase3 03_variadic_templates 第 4 章。
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

                if (stopping_ && tasks_.empty()) return;

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

int main() {
    // Output:
    // === Runtime::spawn ===
    //   answer = 42
    cout << "=== Runtime::spawn ===" << endl;

    Runtime rt(2);
    future<int> answer = rt.spawn([] {
        return 40 + 2;
    });

    cout << "  answer = " << answer.get() << endl << endl;
    return 0;
}
