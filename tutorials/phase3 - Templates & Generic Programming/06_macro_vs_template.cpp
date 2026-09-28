/**
 * Phase 3-6: Macro vs Template / Generic
 *
 * Template 和 macro 都能減少重複 code，但它們處理的層級不同：
 *
 *   Template: compiler 理解的 type-level generic。
 *             有型別檢查、scope、overload resolution、concept constraints。
 *
 *   Macro:    preprocessor 做的 token-level 展開。
 *             可以把 token 轉字串、拼 token、產生宣告，但不懂 C++ 型別。
 *
 * 判斷方式：
 *   - 問題是「同一套邏輯套不同型別」：先想 template。
 *   - 問題是「需要欄位名稱、token 名稱、條件編譯」：macro 或 codegen 才可能做到。
 *   - macro 能不用就不用；它沒有型別安全，debug 成本也高。
 *
 * 目錄:
 *   1. 同一套邏輯套不同型別：優先用 template
 *   2. Macro 的 token stringification: #field
 *   3. Macro 的 token pasting: name ## suffix
 *   4. Macro 的風險：只是文字/token 展開
 */

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
using namespace std;

// ============================================================
// 1. 同一套邏輯套不同型別：優先用 template
// ============================================================
// 本章重點：
//   如果你的目標是「同一套邏輯支援 int、double、string 等不同型別」，
//   template 通常是正確工具。
//
//   compiler 看得懂 template 的型別、scope、overload，
//   所以錯誤比較早被抓到，也比較不容易因為 token 展開造成副作用。
/**
 * 這種「同一個演算法，套不同型別」用 template 比 macro 好。
 * compiler 會做型別檢查，也不會有重複 evaluation 的問題。
 */
template <typename T>
const T& max_value(const T& a, const T& b) {
    // const T& 避免複製，回傳其中一個參數的 reference。
    // 呼叫端要確保傳入的物件生命週期足夠長。
    return (a < b) ? b : a;
}

// 不推薦：macro 只是展開 token，沒有型別檢查，也可能重複執行參數。
// BAD_MAX(x++, y++) 這類呼叫會讓副作用變得難以預測。
#define BAD_MAX(a, b) ((a) < (b) ? (b) : (a))

void template_demo() {
    // Output:
    // === Template: type-level generic ===
    //   max_value(3, 7) = 7
    //   max_value(string("cat"), string("dog")) = dog
    //
    cout << "=== Template: type-level generic ===" << endl;

    cout << "  max_value(3, 7) = " << max_value(3, 7) << endl;
    cout << "  max_value(string(\"cat\"), string(\"dog\")) = "
         << max_value(string("cat"), string("dog")) << endl;

    cout << endl;
}

// ============================================================
// 2. Macro 的 token stringification
// ============================================================
// 本章重點：
//   stringification 是 macro 的 # 操作。
//   它可以把 source code 裡的 token 變成字串。
//
//   這件事 template 做不到：
//     template 只知道型別和值，不知道呼叫端寫的變數名稱或欄位 token。
//
//   所以需要把 user.name 同時變成 "name" 和 user.name 時，
//   macro 才有用武之地。
struct User {
    string name;
    int age;
};

/**
 * #field 會把 field 這個 token 轉成字串。
 *
 * JSON_FIELD(user, name)
 * 展開概念類似：
 *   append_json_field(oss, "name", user.name)
 *
 * Template 做不到這件事，因為 template 拿不到 source code 裡的 token 名稱。
 */
#define JSON_FIELD(obj, field) append_json_field(oss, #field, (obj).field)

template <typename T>
void append_json_field(ostringstream& oss, const string& key, const T& value) {
    // Template 負責「不同 value 型別要怎麼輸出」。
    // Macro 只負責「把欄位 token 轉成 key 字串」。
    // 兩者可以混用，但要讓 macro 的責任越小越好。
    oss << "\"" << key << "\": " << value;
}

void append_json_field(ostringstream& oss, const string& key, const string& value) {
    oss << "\"" << key << "\": \"" << value << "\"";
}

string to_json_like(const User& user) {
    ostringstream oss;
    oss << "{";
    JSON_FIELD(user, name);
    oss << ", ";
    JSON_FIELD(user, age);
    oss << "}";
    return oss.str();
}

void stringification_demo() {
    // Output:
    // === Macro: token stringification ===
    //   {"name": "Alice", "age": 30}
    //
    cout << "=== Macro: token stringification ===" << endl;

    User user{"Alice", 30};
    cout << "  " << to_json_like(user) << endl;
    cout << endl;
}

// ============================================================
// 3. Macro 的 token pasting
// ============================================================
// 本章重點：
//   token pasting 是 macro 的 ## 操作。
//   它可以把兩段 token 拼成新的 token。
//
//   例如 get_ 和 host 拼成 get_host，host 和 _ 拼成 host_。
//   這是 source code 產生技巧，不是 C++ 型別系統的一部分。
//
//   因為 compiler 看到的是 macro 展開後的結果，
//   所以錯誤訊息常會指向展開後的 code，而不是你原本寫 macro 的意圖。
/**
 * ## 可以把 token 拼在一起。
 *
 * MAKE_GETTER(int, age)
 * 展開概念類似：
 *   int get_age() const { return age_; }
 */
#define MAKE_GETTER(type, name) \
    type get_##name() const { return name##_; }
// 注意：macro 產生的是 source token，不受 C++ scope/type system 保護。
// 如果 class 裡沒有 name##_ 這個 member，錯誤會出現在展開後的程式碼。

class Config {
public:
    Config(string host, int port) : host_(std::move(host)), port_(port) {}

    MAKE_GETTER(string, host)
    MAKE_GETTER(int, port)

private:
    string host_;
    int port_;
};

void token_pasting_demo() {
    // Output:
    // === Macro: token pasting ===
    //   config.get_host() = localhost
    //   config.get_port() = 8080
    //
    cout << "=== Macro: token pasting ===" << endl;

    Config config("localhost", 8080);
    cout << "  config.get_host() = " << config.get_host() << endl;
    cout << "  config.get_port() = " << config.get_port() << endl;
    cout << endl;
}

// ============================================================
// 4. Macro 的風險
// ============================================================
// 本章重點：
//   macro 不是函式呼叫，它只是編譯前的 token 替換。
//   所以參數可能被展開多次，副作用也可能發生多次。
//
//   BAD_MAX(next_value(), next_value()) 看起來像呼叫兩次 next_value，
//   但展開後可能呼叫三次或四次，取決於條件走哪個分支。
//
//   這就是為什麼「能用 template/function 解決」時，不要先用 macro。
int next_value() {
    static int value = 0;
    return ++value;
}

void macro_pitfall_demo() {
    // Output:
    // === Macro pitfall ===
    //   template max_value(a, b) = 2
    //   BAD_MAX(next_value(), next_value()) = 5
    //   macro 只是展開 token，不是函式呼叫
    cout << "=== Macro pitfall ===" << endl;

    int a = next_value();
    int b = next_value();
    cout << "  template max_value(a, b) = " << max_value(a, b) << endl;

    // BAD_MAX 可能重複執行參數。下面會呼叫 next_value() 三次，不是兩次。
    // 展開後概念類似：
    //   ((next_value()) < (next_value()) ? (next_value()) : (next_value()))
    // 條件會先用掉兩次呼叫，然後 true/false 分支再用掉其中一次。
    int macro_result = BAD_MAX(next_value(), next_value());
    cout << "  BAD_MAX(next_value(), next_value()) = " << macro_result << endl;
    cout << "  macro 只是展開 token，不是函式呼叫" << endl;
    cout << endl;
}

#undef BAD_MAX
#undef JSON_FIELD
#undef MAKE_GETTER

int main() {
    template_demo();
    stringification_demo();
    token_pasting_demo();
    macro_pitfall_demo();
    return 0;
}
