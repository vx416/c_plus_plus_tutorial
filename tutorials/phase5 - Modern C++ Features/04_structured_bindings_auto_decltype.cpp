/**
 * Phase 5-4: Structured Bindings & auto / decltype
 *
 * 這章處理「型別怎麼被推導」和「怎麼把複合資料拆成名字」。
 *
 * 目錄:
 *   1. structured bindings
 *   2. auto deduction
 *   3. decltype
 *   4. decltype(auto)
 *   5. invoke_result_t
 *   6. 實務使用準則
 */

#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
using namespace std;

// ============================================================
// 1. structured bindings
// ============================================================
// 本章重點：
//   structured binding 可以把 pair、tuple、struct 拆成多個具名變數。
//   常見於 map iteration：
//     for (const auto& [key, value] : map)
void structured_binding_demo() {
    // Output:
    // === structured bindings ===
    //   Alice -> 90
    //   Bob -> 80
    //   tuple item = 1, pen, 2.5
    //
    cout << "=== structured bindings ===" << endl;

    map<string, int> scores{{"Alice", 90}, {"Bob", 80}};
    for (const auto& [name, score] : scores) {
        cout << "  " << name << " -> " << score << endl;
    }

    tuple<int, string, double> item{1, "pen", 2.5};
    auto [id, name, price] = item;
    cout << "  tuple item = " << id << ", " << name << ", " << price << endl;
    cout << endl;
}

// ============================================================
// 2. auto deduction
// ============================================================
// 本章重點：
//   auto 大致採用 template type deduction 規則。
//   auto 會拿掉 top-level const 和 reference。
//   如果要 reference，要明確寫 auto& 或 const auto&。
void auto_demo() {
    // Output:
    // === auto deduction ===
    //   a copy = 10
    //   x after b reference update = 20
    //   c const ref = 20
    //
    cout << "=== auto deduction ===" << endl;

    int x = 10;
    const int& ref = x;

    auto a = ref;
    auto& b = x;
    const auto& c = ref;

    b = 20;
    cout << "  a copy = " << a << endl;
    cout << "  x after b reference update = " << x << endl;
    cout << "  c const ref = " << c << endl;
    cout << endl;
}

// ============================================================
// 3. decltype
// ============================================================
// 本章重點：
//   decltype 取得 expression 的型別。
//   decltype(x) 對變數名稱，得到宣告型別。
//   decltype((x)) 對括號 expression，通常得到 reference 型別，因為 (x) 是 lvalue expression。
void decltype_demo() {
    // Output:
    // === decltype ===
    //   a = 2
    //   x after decltype((x)) ref = 3
    //   decltype((x)) is reference = true
    //
    cout << "=== decltype ===" << endl;

    int x = 1;
    decltype(x) a = 2;
    decltype((x)) b = x;
    b = 3;

    cout << "  a = " << a << endl;
    cout << "  x after decltype((x)) ref = " << x << endl;
    cout << "  decltype((x)) is reference = " << boolalpha
         << is_reference_v<decltype((x))> << noboolalpha << endl;
    cout << endl;
}

// ============================================================
// 4. decltype(auto)
// ============================================================
// 本章重點：
//   auto 回傳值通常會丟掉 reference。
//   decltype(auto) 可以保留 expression 的精確型別，包括 reference。
//
//   寫 wrapper/helper 時，如果要保留原函式回傳 reference，常需要 decltype(auto)。
int global_value = 10;

int& get_global_ref() {
    return global_value;
}

auto get_copy() {
    return get_global_ref();
}

decltype(auto) get_ref() {
    return get_global_ref();
}

void decltype_auto_demo() {
    // Output:
    // === decltype(auto) ===
    //   local copy = 99
    //   global after modifying copy = 10
    //   global after modifying ref = 99
    //
    cout << "=== decltype(auto) ===" << endl;

    auto copy = get_copy();
    copy = 99;
    cout << "  local copy = " << copy << endl;
    cout << "  global after modifying copy = " << global_value << endl;

    decltype(auto) ref = get_ref();
    ref = 99;
    cout << "  global after modifying ref = " << global_value << endl;
    cout << endl;
}

// ============================================================
// 5. invoke_result_t
// ============================================================
// 本章重點：
//   std::invoke_result_t<F, Args...> 會推導「用 Args... 呼叫 F」的回傳型別。
//   它常出現在 generic wrapper、thread pool submit、callback adapter 這類程式碼。
//
//   例如：
//     invoke_result_t<decltype(f), int, int>
//   表示 f(int, int) 的回傳型別。
//
//   如果 callable 不吃參數，常見寫法是：
//     invoke_result_t<F&>
//   表示用 lvalue 形式呼叫 f() 的回傳型別。
template <typename F>
auto call_and_report(F&& f) -> invoke_result_t<F&> {
    using Result = invoke_result_t<F&>;

    cout << "  result is int = " << boolalpha
         << is_same_v<Result, int> << noboolalpha << endl;

    return f();
}

void invoke_result_demo() {
    // Output:
    // === invoke_result_t ===
    //   lambda return type is int = true
    //   result is int = true
    //   value = 42
    //
    cout << "=== invoke_result_t ===" << endl;

    auto make_number = [] {
        return 42;
    };

    using LambdaResult = invoke_result_t<decltype(make_number)&>;
    cout << "  lambda return type is int = " << boolalpha
         << is_same_v<LambdaResult, int> << noboolalpha << endl;

    int value = call_and_report(make_number);
    cout << "  value = " << value << endl;
    cout << endl;
}

// ============================================================
// 6. 實務使用準則
// ============================================================
// 本章重點：
//   auto 適合用在型別很長、右邊已經清楚、或 iterator/ranges 這類型別難寫的地方。
//   public API 回傳型別通常不要濫用 auto，因為讀 header 的人需要知道契約。
//   decltype(auto) 很強，但容易踩 reference 規則，通常只在 forwarding/wrapper 使用。
//   invoke_result_t 適合用在 generic code，要依 callable 自動決定回傳型別時。
void guideline_demo() {
    // Output:
    // === guidelines ===
    //   use auto when it improves readability
    //   use auto& when mutation/reference is intended
    //   use decltype(auto) mainly for wrappers preserving references
    //   use invoke_result_t when generic code needs a callable return type
    cout << "=== guidelines ===" << endl;
    cout << "  use auto when it improves readability" << endl;
    cout << "  use auto& when mutation/reference is intended" << endl;
    cout << "  use decltype(auto) mainly for wrappers preserving references" << endl;
    cout << "  use invoke_result_t when generic code needs a callable return type" << endl;
    cout << endl;
}

int main() {
    structured_binding_demo();
    auto_demo();
    decltype_demo();
    decltype_auto_demo();
    invoke_result_demo();
    guideline_demo();
    return 0;
}
