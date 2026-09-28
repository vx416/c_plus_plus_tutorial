/**
 * Phase 7-2: Preprocessor & Macro
 *
 * Macro 是 preprocessor 做的 token 展開，不是 C++ 型別系統的一部分。
 *
 * 目錄:
 *   1. object-like macro
 *   2. function-like macro
 *   3. stringification #
 *   4. token pasting ##
 *   5. conditional compilation
 */

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#define APP_VERSION 7
#define BAD_SQUARE(x) ((x) * (x))
#define TO_STRING(x) #x
#define LOG_EXPR(expr) cout << #expr << " = " << (expr) << endl
#define MAKE_NAME(prefix, name) prefix##_##name
#define JSON_GETTER_FIELD(obj, field) append_json_field(oss, #field, (obj).get_##field())

// ============================================================
// 1. object-like macro
// ============================================================
// 本章重點：
//   object-like macro 像文字常數。
//   但能用 constexpr 就優先用 constexpr，因為 compiler 看得懂型別。
void object_macro_demo() {
    // Output:
    // === object-like macro ===
    //   APP_VERSION = 7
    //
    cout << "=== object-like macro ===" << endl;
    cout << "  APP_VERSION = " << APP_VERSION << endl << endl;
}

// ============================================================
// 2. function-like macro
// ============================================================
// 本章重點：
//   function-like macro 看起來像函式，但只是 token 展開。
//   參數可能被使用多次，所以帶副作用的參數很危險。
constexpr int square(int x) {
    return x * x;
}

void function_macro_demo() {
    // Output:
    // === function-like macro ===
    //   BAD_SQUARE(x) = 9
    //   square(x) = 9
    //
    //   side-effect argument: BAD_SQUARE(next_x())
    //     expected like function call: 3 * 3 = 9, x becomes 4
    //     actual macro expansion: next_x() * next_x()
    //     actual result = 12, x = 5
    //
    cout << "=== function-like macro ===" << endl;
    int x = 3;
    cout << "  BAD_SQUARE(x) = " << BAD_SQUARE(x) << endl;
    cout << "  square(x) = " << square(x) << endl << endl;

    x = 3;
    auto next_x = [&] {
        return x++;
    };

    cout << "  side-effect argument: BAD_SQUARE(next_x())" << endl;
    cout << "    expected like function call: 3 * 3 = 9, x becomes 4" << endl;
    cout << "    actual macro expansion: next_x() * next_x()" << endl;
    cout << "    actual result = " << BAD_SQUARE(next_x()) << ", x = " << x << endl;
    cout << endl;
}

// ============================================================
// 3. stringification #
// ============================================================
// 本章重點：
//   # 可以把 token 轉成字串。
//   這是 template 做不到的，因為 template 不知道 source token 名稱。
void stringification_demo() {
    // Output:
    // === stringification ===
    //   TO_STRING(hello_world) = hello_world
    //   LOG_EXPR(x + y): x + y = 7
    //
    cout << "=== stringification ===" << endl;
    cout << "  TO_STRING(hello_world) = " << TO_STRING(hello_world) << endl;

    int x = 3;
    int y = 4;
    cout << "  LOG_EXPR(x + y): ";
    LOG_EXPR(x + y);
    cout << endl;
}

// ============================================================
// 4. token pasting ##
// ============================================================
// 本章重點：
//   ## 可以把 token 拼成新 token。
//   它常用於少量 code generation，但大型邏輯不要塞進 macro。
struct UserDto {
    string name;
    int score;

    const string& get_name() const {
        return name;
    }

    int get_score() const {
        return score;
    }
};

void append_json_value(ostringstream& oss, const string& value) {
    oss << "\"" << value << "\"";
}

void append_json_value(ostringstream& oss, int value) {
    oss << value;
}

template <typename T>
void append_json_field(ostringstream& oss, const string& key, const T& value) {
    oss << "\"" << key << "\": ";
    append_json_value(oss, value);
}

string marshal_user_json(const UserDto& user) {
    ostringstream oss;
    oss << "{";
    JSON_GETTER_FIELD(user, name);   // #field -> "name", get_##field -> get_name()
    oss << ", ";
    JSON_GETTER_FIELD(user, score);  // #field -> "score", get_##field -> get_score()
    oss << "}";
    return oss.str();
}

void token_pasting_demo() {
    // Output:
    // === token pasting ===
    //   user_id = 42
    //   user json = {"name": "Alice", "score": 95}
    //
    cout << "=== token pasting ===" << endl;
    int MAKE_NAME(user, id) = 42;
    cout << "  user_id = " << user_id << endl;

    UserDto user{"Alice", 95};
    cout << "  user json = " << marshal_user_json(user) << endl << endl;
}

// ============================================================
// 5. conditional compilation
// ============================================================
// 本章重點：
//   #if / #ifdef 在編譯前決定哪些 code 存在。
//   適合平台差異、feature flag、debug build。
void conditional_demo() {
    // Output:
    // === conditional compilation ===
    //   debug-like build
    cout << "=== conditional compilation ===" << endl;
#ifdef NDEBUG
    cout << "  release-like build" << endl;
#else
    cout << "  debug-like build" << endl;
#endif
    cout << endl;
}

int main() {
    object_macro_demo();
    function_macro_demo();
    stringification_demo();
    token_pasting_demo();
    conditional_demo();
    return 0;
}

#undef APP_VERSION
#undef BAD_SQUARE
#undef TO_STRING
#undef LOG_EXPR
#undef MAKE_NAME
#undef JSON_GETTER_FIELD
