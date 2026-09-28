/**
 * 練習 5: runtime pipeline
 *
 * 用 future 把兩段工作串起來：load -> transform。
 */

#include <cassert>
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

string uppercase(string input) {
    for (char& ch : input) {
        if (ch >= 'a' && ch <= 'z') {
            ch = static_cast<char>(ch - 'a' + 'A');
        }
    }
    return input;
}

int main() {
    Runtime rt(2);

    auto loaded = rt.spawn([] {
        return string("mini runtime");
    });

    string text = loaded.get();
    auto transformed = rt.spawn([text] {
        return uppercase(text);
    });

    assert(transformed.get() == "MINI RUNTIME");

    cout << "ex05 passed!" << endl;
    return 0;
}
