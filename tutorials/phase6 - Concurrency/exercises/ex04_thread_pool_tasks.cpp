/**
 * 練習 4: Thread Pool Tasks
 *
 * 用自寫 ThreadPool 平行計算平方和。
 *
 * 重點練習：
 *   - std::async 不能指定 thread pool
 *   - ThreadPool 用固定 worker threads + task queue
 *   - packaged_task 把 task 結果接到 future
 */

#include <cassert>
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

int square_sum_with_pool(const vector<int>& values) {
    ThreadPool pool(2);
    vector<future<int>> futures;

    for (int value : values) {
        futures.push_back(pool.submit([value] {
            return value * value;
        }));
    }

    int total = 0;
    for (future<int>& future : futures) {
        total += future.get();
    }
    return total;
}

int main() {
    assert(square_sum_with_pool({1, 2, 3, 4}) == 30);
    cout << "ex04 passed!" << endl;
    return 0;
}
