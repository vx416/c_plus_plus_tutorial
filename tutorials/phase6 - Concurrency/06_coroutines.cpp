/**
 * Phase 6-6: C++20 Coroutines
 *
 * Coroutine 是語言層 state machine。
 * 它讓函式可以暫停、恢復，並用 co_return / co_yield / co_await 表達控制流程。
 *
 * 重要：
 *   C++20 coroutine 不是 Tokio-style runtime。
 *   標準庫提供 coroutine building blocks，但沒有通用 executor、reactor、thread pool。
 *
 * 目錄:
 *   1. coroutine 不是 thread
 *   2. co_return: 回傳一個 Task
 *   3. co_yield: Generator
 *   4. co_await: awaiter 協定
 *   5. 為什麼還需要 runtime/library
 */

#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <utility>
using namespace std;

// ============================================================
// 1. coroutine 不是 thread
// ============================================================
// 本章重點：
//   coroutine 不會自動開 thread。
//   它只是把函式轉成可暫停/恢復的 state machine。
//
//   是否在 event loop、thread pool、I/O reactor 上跑，取決於你使用的 coroutine type/library。
void coroutine_not_thread_demo() {
    // Output:
    // === coroutine is not a thread ===
    //   coroutine is a resumable state machine
    //   standard C++ does not provide a Tokio-like runtime
    //
    cout << "=== coroutine is not a thread ===" << endl;
    cout << "  coroutine is a resumable state machine" << endl;
    cout << "  standard C++ does not provide a Tokio-like runtime" << endl;
    cout << endl;
}

// ============================================================
// 2. co_return: 回傳一個 Task
// ============================================================
// 本章重點：
//   要讓函式使用 co_return，你必須定義 return type 的 promise_type。
//   promise_type 是 coroutine 和外部結果物件之間的橋樑。
//
//   這個 SimpleTask 是最小同步示範：
//     answer() 建立 coroutine
//     co_return 42 把值放進 promise
//     task.value() 讀出結果
class SimpleTask {
public:
    struct promise_type {
        int value = 0;
        exception_ptr error;

        SimpleTask get_return_object() {
            return SimpleTask(coroutine_handle<promise_type>::from_promise(*this));
        }

        suspend_never initial_suspend() noexcept { return {}; }
        suspend_always final_suspend() noexcept { return {}; }

        void return_value(int v) {
            value = v;
        }

        void unhandled_exception() {
            error = current_exception();
        }
    };

    explicit SimpleTask(coroutine_handle<promise_type> handle) : handle_(handle) {}

    SimpleTask(const SimpleTask&) = delete;
    SimpleTask& operator=(const SimpleTask&) = delete;

    SimpleTask(SimpleTask&& other) noexcept : handle_(exchange(other.handle_, {})) {}

    ~SimpleTask() {
        // final_suspend 是 suspend_always，coroutine frame 會留著讓 SimpleTask 讀 promise。
        // SimpleTask 生命週期結束時再 destroy frame。
        if (handle_) {
            handle_.destroy();
        }
    }

    int value() const {
        if (handle_.promise().error) {
            rethrow_exception(handle_.promise().error);
        }
        return handle_.promise().value;
    }

private:
    coroutine_handle<promise_type> handle_;
};

SimpleTask answer() {
    co_return 42;
}

void co_return_demo() {
    // Output:
    // === co_return ===
    //   answer = 42
    //
    cout << "=== co_return ===" << endl;
    SimpleTask task = answer();
    cout << "  answer = " << task.value() << endl;
    cout << endl;
}

// ============================================================
// 3. co_yield: Generator
// ============================================================
// 本章重點：
//   co_yield 適合做 lazy sequence。
//   coroutine 每次 yield 一個值後暫停；呼叫端 resume 後繼續跑到下一個 yield。
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

void co_yield_demo() {
    // Output:
    // === co_yield generator ===
    //   values: 1 2 3
    //
    cout << "=== co_yield generator ===" << endl;

    IntGenerator gen = range(1, 4);
    cout << "  values:";
    while (optional<int> value = gen.next()) {
        cout << ' ' << *value;
    }
    cout << endl << endl;
}

// ============================================================
// 4. co_await: awaiter 協定
// ============================================================
// 本章重點：
//   co_await 會看 awaiter 的三個動作：
//     await_ready()   是否不用暫停
//     await_suspend() 暫停時做什麼
//     await_resume()  恢復後產生什麼值
//
//   真正 async I/O library 會在 await_suspend 裡把 coroutine handle 交給 event loop。
//   這裡只示範協定形狀，不做 I/O runtime。
struct ImmediateInt {
    int value;

    bool await_ready() const noexcept {
        return true;
    }

    void await_suspend(coroutine_handle<>) const noexcept {}

    int await_resume() const noexcept {
        return value;
    }
};

SimpleTask await_immediate() {
    int value = co_await ImmediateInt{7};
    co_return value * 2;
}

void co_await_demo() {
    // Output:
    // === co_await awaiter protocol ===
    //   awaited value = 14
    //
    cout << "=== co_await awaiter protocol ===" << endl;
    SimpleTask task = await_immediate();
    cout << "  awaited value = " << task.value() << endl;
    cout << endl;
}

// ============================================================
// 5. 為什麼還需要 runtime/library
// ============================================================
// 本章重點：
//   coroutine 只定義「如何暫停/恢復」。
//   誰負責在 socket ready、timer 到期、thread pool task 完成時 resume？
//   標準庫沒有提供通用答案。
//
//   實務上通常使用：
//     Boost.Asio / standalone Asio
//     folly coro
//     cppcoro
//     自己的 game/server event loop
void runtime_demo() {
    // Output:
    // === coroutine needs a scheduler for real async I/O ===
    //   language: coroutine state machine
    //   library/runtime: executor, event loop, timers, I/O readiness
    cout << "=== coroutine needs a scheduler for real async I/O ===" << endl;
    cout << "  language: coroutine state machine" << endl;
    cout << "  library/runtime: executor, event loop, timers, I/O readiness" << endl;
    cout << endl;
}

int main() {
    coroutine_not_thread_demo();
    co_return_demo();
    co_yield_demo();
    co_await_demo();
    runtime_demo();
    return 0;
}
