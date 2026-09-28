/**
 * Phase 11-4: Mini Tokio-style Runtime
 *
 * 這是一個 task runtime，不是完整 Rust Tokio clone：
 *   - spawn callable，回傳 future
 *   - packaged_task 接住 return/exception
 *   - fixed worker threads 執行 queue 裡的 task
 *   - join_all 等多個 task 完成
 */

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>
using namespace std;
using namespace std::chrono_literals;

class MiniTokio {
public:
    explicit MiniTokio(size_t workers) {
        for (size_t i = 0; i < workers; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~MiniTokio() {
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

template <typename T>
vector<T> join_all(vector<future<T>>& futures) {
    vector<T> results;
    results.reserve(futures.size());
    for (future<T>& future : futures) {
        results.push_back(future.get());
    }
    return results;
}

int simulated_io(int id) {
    this_thread::sleep_for(5ms);
    return id * 10;
}

int main() {
    // Output:
    // === mini Tokio-style runtime ===
    //   joined total = 100
    //   model = packaged_task + future + worker queue
    cout << "=== mini Tokio-style runtime ===" << endl;

    MiniTokio rt(3);
    vector<future<int>> tasks;
    for (int id : {1, 2, 3, 4}) {
        tasks.push_back(rt.spawn([id] {
            return simulated_io(id);
        }));
    }

    vector<int> values = join_all(tasks);
    int total = 0;
    for (int value : values) total += value;

    cout << "  joined total = " << total << endl;
    cout << "  model = packaged_task + future + worker queue" << endl << endl;
    return 0;
}
