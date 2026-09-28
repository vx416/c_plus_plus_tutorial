/**
 * Phase 5-5: Error Handling
 *
 * C++ 常見錯誤處理方式：
 *   exception: 失敗直接跳出目前控制流程
 *   error code: 呼叫端檢查回傳狀態
 *   optional: 只表達有/沒有，不說明原因
 *   expected-style result: 成功有值，失敗有錯誤原因
 *
 * 目錄:
 *   1. exceptions
 *   2. error code / bool return
 *   3. optional
 *   4. expected-style result
 *   5. 選擇準則
 */

#include <charconv>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <variant>
using namespace std;

bool parse_int_raw(string_view text, int& output) {
    const char* begin = text.data();
    const char* end = text.data() + text.size();
    auto [ptr, ec] = from_chars(begin, end, output);
    return ec == errc{} && ptr == end;
}

// ============================================================
// 1. exceptions
// ============================================================
// 本章重點：
//   exception 適合「正常流程不預期發生」的錯誤。
//   好處是呼叫鏈中間不用每層傳錯誤碼。
//   代價是控制流程比較隱性，API 文件要清楚說明可能丟什麼。
int parse_int_or_throw(string_view text) {
    int value = 0;
    if (!parse_int_raw(text, value)) {
        throw invalid_argument("invalid integer");
    }
    return value;
}

void exception_demo() {
    // Output:
    // === exceptions ===
    //   parse 42 = 42
    //   caught error: invalid integer
    //
    cout << "=== exceptions ===" << endl;

    try {
        int value = parse_int_or_throw("42");
        cout << "  parse 42 = " << value << endl;
        value = parse_int_or_throw("xx");
        cout << "  parse xx = " << value << endl;
    } catch (const invalid_argument& ex) {
        cout << "  caught error: " << ex.what() << endl;
    }
    cout << endl;
}

// ============================================================
// 2. error code / bool return
// ============================================================
// 本章重點：
//   error code 讓錯誤變成顯式回傳值。
//   呼叫端每次都要檢查，適合底層、效能敏感、或不想用 exception 的 API。
bool parse_int_bool(string_view text, int& output) {
    return parse_int_raw(text, output);
}

void error_code_demo() {
    // Output:
    // === error code / bool ===
    //   parsed = 123
    //   parse bad ok = false
    //
    cout << "=== error code / bool ===" << endl;

    int value = 0;
    if (parse_int_bool("123", value)) {
        cout << "  parsed = " << value << endl;
    }
    cout << "  parse bad ok = " << boolalpha << parse_int_bool("bad", value)
         << noboolalpha << endl;
    cout << endl;
}

// ============================================================
// 3. optional
// ============================================================
// 本章重點：
//   optional<T> 適合「失敗原因不重要，只需要知道有沒有值」。
//   例如查找資料可能找不到。
//   如果你需要錯誤訊息，optional 不夠，應該用 expected-style result。
optional<int> parse_int_optional(string_view text) {
    int value = 0;
    if (!parse_int_raw(text, value)) {
        return nullopt;
    }
    return value;
}

void optional_demo() {
    // Output:
    // === optional ===
    //   has value = true
    //
    cout << "=== optional ===" << endl;

    optional<int> value = parse_int_optional("77");
    cout << "  has value = " << boolalpha << value.has_value() << noboolalpha << endl;
    cout << endl;
}

// ============================================================
// 4. expected-style result
// ============================================================
// 本章重點：
//   C++23 std::expected<T, E> 表示：
//     成功時有 T
//     失敗時有 E
//
//   這個檔案用 SimpleExpected 示範同樣概念，避免不同標準庫尚未支援 std::expected。
template <typename T, typename E>
class SimpleExpected {
public:
    static SimpleExpected success(T value) {
        return SimpleExpected(SuccessTag{}, std::move(value));
    }

    static SimpleExpected failure(E error) {
        return SimpleExpected(FailureTag{}, std::move(error));
    }

    bool has_value() const { return has_value_; }

    const T& value() const { return get<T>(data_); }
    const E& error() const { return get<E>(data_); }

private:
    struct SuccessTag {};
    struct FailureTag {};

    SimpleExpected(SuccessTag, T value) : data_(std::move(value)), has_value_(true) {}
    SimpleExpected(FailureTag, E error) : data_(std::move(error)), has_value_(false) {}

    variant<T, E> data_;
    bool has_value_;
};

SimpleExpected<int, string> parse_int_expected(string_view text) {
    int value = 0;
    if (!parse_int_raw(text, value)) {
        return SimpleExpected<int, string>::failure("not a valid integer");
    }
    return SimpleExpected<int, string>::success(value);
}

void expected_demo() {
    // Output:
    // === expected-style result ===
    //   value = 100
    //   error = not a valid integer
    //
    cout << "=== expected-style result ===" << endl;

    auto ok = parse_int_expected("100");
    if (ok.has_value()) {
        cout << "  value = " << ok.value() << endl;
    }

    auto bad = parse_int_expected("abc");
    if (!bad.has_value()) {
        cout << "  error = " << bad.error() << endl;
    }
    cout << endl;
}

// ============================================================
// 5. 選擇準則
// ============================================================
// 本章重點：
//   exception: 例外情況，想中斷控制流程。
//   error code: 底層 API、效能敏感、必須顯式處理。
//   optional: 只有有/沒有，沒有錯誤原因。
//   expected: 錯誤是正常可能結果，而且要帶錯誤原因。
void choice_demo() {
    // Output:
    // === error handling choice ===
    //   exceptional failure: exception
    //   low-level status: error code
    //   maybe missing: optional
    //   value or error reason: expected-style result
    cout << "=== error handling choice ===" << endl;
    cout << "  exceptional failure: exception" << endl;
    cout << "  low-level status: error code" << endl;
    cout << "  maybe missing: optional" << endl;
    cout << "  value or error reason: expected-style result" << endl;
    cout << endl;
}

int main() {
    exception_demo();
    error_code_demo();
    optional_demo();
    expected_demo();
    choice_demo();
    return 0;
}
