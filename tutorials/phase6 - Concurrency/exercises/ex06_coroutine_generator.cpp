/**
 * 練習 6: Coroutine Generator
 *
 * 用 co_yield 實作一個整數 generator。
 *
 * 重點練習：
 *   - coroutine promise_type
 *   - co_yield 暫停並交出值
 *   - 呼叫端用 next() resume coroutine
 */

#include <cassert>
#include <coroutine>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>
using namespace std;

class IntGenerator {
public:
    struct promise_type {
        int current = 0;

        IntGenerator get_return_object() {
            return IntGenerator(coroutine_handle<promise_type>::from_promise(*this));
        }

        suspend_always initial_suspend() noexcept { return {}; }
        suspend_always final_suspend() noexcept { return {}; }

        suspend_always yield_value(int value) noexcept {
            current = value;
            return {};
        }

        void return_void() {}
        void unhandled_exception() { terminate(); }
    };

    explicit IntGenerator(coroutine_handle<promise_type> handle) : handle_(handle) {}

    IntGenerator(const IntGenerator&) = delete;
    IntGenerator& operator=(const IntGenerator&) = delete;

    IntGenerator(IntGenerator&& other) noexcept : handle_(exchange(other.handle_, {})) {}

    ~IntGenerator() {
        if (handle_) {
            handle_.destroy();
        }
    }

    optional<int> next() {
        if (!handle_ || handle_.done()) {
            return nullopt;
        }
        handle_.resume();
        if (handle_.done()) {
            return nullopt;
        }
        return handle_.promise().current;
    }

private:
    coroutine_handle<promise_type> handle_;
};

IntGenerator range(int begin, int end) {
    for (int value = begin; value < end; ++value) {
        co_yield value;
    }
}

int main() {
    IntGenerator generator = range(2, 6);
    vector<int> values;

    while (optional<int> value = generator.next()) {
        values.push_back(*value);
    }

    assert((values == vector<int>{2, 3, 4, 5}));
    cout << "ex06 passed!" << endl;
    return 0;
}
