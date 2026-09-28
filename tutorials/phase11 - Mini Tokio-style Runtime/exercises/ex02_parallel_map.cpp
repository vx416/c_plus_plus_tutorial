/**
 * 練習 2: parallel map
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

class Runtime {
public:
    explicit Runtime(size_t workers) {
        for (size_t i = 0; i < workers; ++i) workers_.emplace_back([this] { loop(); });
    }

    ~Runtime() {
        {
            lock_guard<mutex> lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();
        for (thread& worker : workers_) worker.join();
    }

    template <typename F>
    auto spawn(F&& f) -> future<invoke_result_t<F&>> {
        using Result = invoke_result_t<F&>;
        auto task = make_shared<packaged_task<Result()>>(std::forward<F>(f));
        future<Result> result = task->get_future();
        {
            lock_guard<mutex> lock(mutex_);
            tasks_.push([task] { (*task)(); });
        }
        cv_.notify_one();
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
    queue<function<void()>> tasks_;
    mutex mutex_;
    condition_variable cv_;
    bool stopping_ = false;
};

vector<int> parallel_square(Runtime& rt, const vector<int>& values) {
    vector<future<int>> futures;
    for (int value : values) {
        futures.push_back(rt.spawn([value] { return value * value; }));
    }

    vector<int> results;
    for (future<int>& future : futures) {
        results.push_back(future.get());
    }
    return results;
}

int main() {
    Runtime rt(2);
    vector<int> result = parallel_square(rt, {1, 2, 3, 4});

    assert((result == vector<int>{1, 4, 9, 16}));

    cout << "ex02 passed!" << endl;
    return 0;
}
