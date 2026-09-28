/**
 * 練習 5: Parse Result
 *
 * parse int 時回傳成功值或錯誤訊息。
 *
 * 重點練習：
 *   - expected-style result
 *   - 不用 exception 也能回傳錯誤原因
 *   - 呼叫端必須顯式檢查 has_value()
 */

#include <cassert>
#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>
#include <variant>
using namespace std;

template <typename T, typename E>
class Result {
public:
    static Result ok(T value) {
        return Result(OkTag{}, std::move(value));
    }

    static Result err(E error) {
        return Result(ErrTag{}, std::move(error));
    }

    bool has_value() const { return has_value_; }
    const T& value() const { return get<T>(data_); }
    const E& error() const { return get<E>(data_); }

private:
    struct OkTag {};
    struct ErrTag {};

    Result(OkTag, T value) : data_(std::move(value)), has_value_(true) {}
    Result(ErrTag, E error) : data_(std::move(error)), has_value_(false) {}

    variant<T, E> data_;
    bool has_value_;
};

Result<int, string> parse_int(string_view text) {
    // TODO: 使用 from_chars，不丟 exception。
    int value = 0;
    const char* begin = text.data();
    const char* end = text.data() + text.size();
    auto [ptr, ec] = from_chars(begin, end, value);
    if (ec != errc{} || ptr != end) {
        return Result<int, string>::err("invalid integer");
    }
    return Result<int, string>::ok(value);
}

int main() {
    auto ok = parse_int("42");
    assert(ok.has_value());
    assert(ok.value() == 42);

    auto bad = parse_int("4x");
    assert(!bad.has_value());
    assert(bad.error() == "invalid integer");

    cout << "ex05 passed!" << endl;
    return 0;
}
