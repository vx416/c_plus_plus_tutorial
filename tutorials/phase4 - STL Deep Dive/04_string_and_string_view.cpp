/**
 * Phase 4-4: std::string & std::string_view
 *
 * string 擁有字串資料；string_view 只是看一段字串。
 * string_view 很快，因為不複製；也很危險，因為它不延長原字串生命週期。
 *
 * 目錄:
 *   1. string owns data
 *   2. string_view does not own data
 *   3. lifetime 陷阱
 *   4. substring without allocation
 *   5. API 設計準則
 */

#include <iostream>
#include <string>
#include <string_view>
using namespace std;

// ============================================================
// 1. string owns data
// ============================================================
// 本章重點：
//   std::string 擁有字串內容，會管理記憶體。
//   複製 string 會複製內容；move string 可以把資源搬走。
//   SSO 是 small string optimization：短字串可能直接存在 string 物件內，不上 heap。
void string_demo() {
    // Output:
    // === string owns data ===
    //   original = Alice
    //   copy     = Alice Smith
    //
    cout << "=== string owns data ===" << endl;

    string name = "Alice";
    string copy = name;
    copy += " Smith";

    cout << "  original = " << name << endl;
    cout << "  copy     = " << copy << endl;
    cout << endl;
}

// ============================================================
// 2. string_view does not own data
// ============================================================
// 本章重點：
//   string_view 只存 pointer + length。
//   它不配置、不複製，也不保證 null-terminated。
//   適合用在「只讀、不保存」的函式參數。
void print_label(string_view label) {
    cout << "  label = " << label << ", size = " << label.size() << endl;
}

void string_view_demo() {
    // Output:
    // === string_view does not own data ===
    //   label = hello, size = 5
    //   label = literal, size = 7
    //
    cout << "=== string_view does not own data ===" << endl;

    string owned = "hello";
    print_label(owned);
    print_label("literal");
    cout << endl;
}

// ============================================================
// 3. lifetime 陷阱
// ============================================================
// 本章重點：
//   string_view 不能比原始字串活得久。
//   如果 string_view 指向 temporary string，temporary 消失後 view 就懸空。
//
//   這段危險 code 不執行，只留著當警告：
//     string_view bad = string("temporary");
//     cout << bad; // undefined behavior
//
//   延伸：函式參數收 string_view，不代表成員也能存 string_view。
//     class User { string_view name_; User(string_view n) : name_(n) {} };  // 呼叫端的字串一死就懸空
//     class User { string name_;      User(string_view n) : name_(n) {} };  // 正確：收 view，存 string
string make_name() {
    return "temporary";
}

void lifetime_demo() {
    // Output:
    // === string_view lifetime ===
    //   ok view = temporary
    //   rule: store string_view only if the referenced string outlives it
    //
    cout << "=== string_view lifetime ===" << endl;

    string stable = make_name();
    string_view ok = stable;
    cout << "  ok view = " << ok << endl;
    cout << "  rule: store string_view only if the referenced string outlives it" << endl;
    cout << endl;
}

// ============================================================
// 4. substring without allocation
// ============================================================
// 本章重點：
//   string::substr 會建立新 string，通常需要配置/複製。
//   string_view::substr 只是調整 pointer + length，不複製字元。
string_view value_after_equal(string_view line) {
    size_t pos = line.find('=');
    if (pos == string_view::npos) {
        return {};
    }
    return line.substr(pos + 1);
}

void substring_demo() {
    // Output:
    // === substring without allocation ===
    //   value = localhost
    //
    cout << "=== substring without allocation ===" << endl;

    string config = "host=localhost";
    string_view value = value_after_equal(config);
    cout << "  value = " << value << endl;
    cout << endl;
}

// ============================================================
// 5. API 設計準則
// ============================================================
// 本章重點：
//   函式只需要讀字串，且不保存：用 string_view。
//   函式需要擁有或保存字串：用 string。
//   函式需要 C API 的 null-terminated 字串：用 string 或 const char*，不要假設 string_view 有 '\0'。
//
//   setter / constructor 收 string_view 的取捨（sink parameter 完整討論見 phase5 01_move_semantics 第 8 章）：
//     + 呼叫端傳字面值或子字串時，不用先建一個暫時 string
//     + 存進 string 成員時走 assign，capacity 夠一樣會重用舊 buffer
//     - 收到 rvalue string 也只能 copy，string_view 沒有所有權，接管不了 buffer
//     - 不能跟 string&& overload 配對：字面值轉 string_view 和轉 string 都是一次 user-defined conversion，
//       優先度相同，編譯器不選：
//         void set(string_view);  void set(string&&);
//         set("x");               // error: call to 'set' is ambiguous
//       要吃 rvalue 就用傳統的 const string& 加 string&& 那對。
//   底下的 User 用 by-value string 收，是 sink 的預設寫法；改收 string_view 也可以，看呼叫端主要傳什麼。
class User {
public:
    explicit User(string name) : name_(std::move(name)) {}

    string_view name() const { return name_; }

private:
    string name_;
};

void api_design_demo() {
    // Output:
    // === API design ===
    //   label = Carol, size = 5
    //   User stores string, exposes read-only string_view
    cout << "=== API design ===" << endl;

    User user("Carol");
    print_label(user.name());
    cout << "  User stores string, exposes read-only string_view" << endl;
    cout << endl;
}

int main() {
    string_demo();
    string_view_demo();
    lifetime_demo();
    substring_demo();
    api_design_demo();
    return 0;
}
