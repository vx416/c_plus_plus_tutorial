/**
 * Phase 3-5: Concepts (C++20)
 *
 * Concept 用來描述 template 參數必須滿足的條件。
 * 它比 SFINAE 更直覺，錯誤訊息通常也更好懂。
 * 可以把 concept 想成「template 的型別約束」：
 *   template <integral T> 代表 T 必須符合 integral。
 * 這跟 Go generic constraint 類似，但 C++ concepts 檢查的是 expression 是否合法、
 * 型別特性是否滿足，不是 Go 的 underlying type set 語意。
 *
 * 目錄:
 *   1. 標準 library concepts
 *   2. 自訂 concept
 *   3. requires expression
 *   4. concept vs if constexpr
 */

#include <concepts>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. 標準 library concepts
// ============================================================
// 本章重點：
//   standard concepts 是標準 library 已經幫你定義好的型別限制。
//   你不用自己寫「是不是整數」「是不是浮點數」的判斷。
//
//   template <integral T> 的意思是：
//     這個 function template 只接受符合 integral 的 T。
//
//   如果呼叫 double_integer(3.14)，編譯器會在進入 function body 前就拒絕。
template <integral T>
T double_integer(T value) {
    // integral 是 <concepts> 提供的標準 concept。
    // int、long、unsigned、bool 都符合 integral；如果不想接受 bool，要額外排除。
    return value * 2;
}

template <floating_point T>
T half_float(T value) {
    // floating_point 只接受 float、double、long double 這類浮點型別。
    return value / 2;
}

void standard_concepts_demo() {
    // Output:
    // === 標準 concepts ===
    //   double_integer(21) = 42
    //   half_float(3.0) = 1.5
    //
    cout << "=== 標準 concepts ===" << endl;

    cout << "  double_integer(21) = " << double_integer(21) << endl;
    cout << "  half_float(3.0) = " << half_float(3.0) << endl;

    // double_integer(3.14); // 編譯錯誤：double 不是 integral
    cout << endl;
}

// ============================================================
// 2. 自訂 concept
// ============================================================
// 本章重點：
//   自訂 concept 是把「這個型別必須能做什麼」命名起來。
//   例如 Addable 代表：
//     對 T 來說，a + b 必須能編譯。
//
//   好處是錯誤訊息和程式意圖都比較清楚。
//   看到 template <Addable T>，就知道這個 template 需要 T 能相加。
template <typename T>
concept Addable = requires(T a, T b) {
    // requires expression 檢查「對 T 來說，a + b 這個 expression 是否合法」。
    // 這裡沒有要求回傳型別是什麼，只要求能編譯。
    a + b;
};

template <Addable T>
T add_twice(T a, T b) {
    // 因為 Addable 已經保證 a + b 合法，function body 裡可以直接使用 +。
    // 但這裡回傳 T，因此 (a + b) + (a + b) 的結果也需要能轉回 T。
    return (a + b) + (a + b);
}

void custom_concept_demo() {
    // Output:
    // === 自訂 concept ===
    //   add_twice(1, 2) = 6
    //   add_twice(string("a"), string("b")) = abab
    //
    cout << "=== 自訂 concept ===" << endl;

    cout << "  add_twice(1, 2) = " << add_twice(1, 2) << endl;
    cout << "  add_twice(string(\"a\"), string(\"b\")) = "
         << add_twice(string("a"), string("b")) << endl;
    cout << endl;
}

// ============================================================
// 3. requires expression
// ============================================================
// 本章重點：
//   requires expression 是 concept 裡用來檢查 expression 的語法。
//   它不是執行 value.size()，而是在編譯期問：
//     如果 T 是某個型別，value.size() 這段 code 合不合法？
//
//   這裡還進一步要求 size() 的結果可以轉成 size_t。
//   所以 HasSize 不只是「有 size 這個名字」，還要求回傳值型別合理。
template <typename T>
concept HasSize = requires(const T& value) {
    // compound requirement：
    //   { value.size() } -> convertible_to<size_t>;
    //
    // 表示 value.size() 必須是合法 expression，
    // 而且它的結果要能轉成 size_t。
    { value.size() } -> convertible_to<size_t>;
};

template <HasSize T>
void print_size(const T& value) {
    cout << "  size = " << value.size() << endl;
}

void requires_demo() {
    // Output:
    // === requires expression ===
    //   size = 5
    //   size = 3
    //
    cout << "=== requires expression ===" << endl;

    string text = "hello";
    vector<int> nums{1, 2, 3};
    print_size(text);
    print_size(nums);

    cout << endl;
}

// ============================================================
// 4. concept vs if constexpr
// ============================================================
// 本章重點：
//   concept 可以用在兩種常見位置：
//
//   1. 放在 template 宣告上：不符合就不准呼叫。
//      例如 template <HasSize T> void print_size(...)
//
//   2. 放在 if constexpr 裡：任何型別都能呼叫，但不同型別走不同分支。
//      例如 debug_print(Hidden{}) 不報錯，只印 fallback 訊息。
//
//   簡單判斷：
//     不符合就應該禁止使用 -> 放在 template 宣告上。
//     不符合也有替代行為 -> 搭配 if constexpr。
template <typename T>
concept Printable = requires(ostream& os, const T& value) {
    // 檢查 os << value 是否合法。
    // 這種 concept 很適合用來限制 logging/debug helper。
    os << value;
};

template <typename T>
void debug_print(const T& value) {
    // concept 不一定只能放在 template 宣告上，也可以配合 if constexpr 做分派。
    // 這個版本接受任何 T；可印的就印，不可印的走 fallback。
    if constexpr (Printable<T>) {
        cout << "  value = " << value << endl;
    } else {
        cout << "  value is not printable" << endl;
    }
}

struct Hidden {};

void concept_if_constexpr_demo() {
    // Output:
    // === concept + if constexpr ===
    //   value = 42
    //   value = template
    //   value is not printable
    cout << "=== concept + if constexpr ===" << endl;

    debug_print(42);
    debug_print(string("template"));
    debug_print(Hidden{});
    cout << endl;
}

int main() {
    standard_concepts_demo();
    custom_concept_demo();
    requires_demo();
    concept_if_constexpr_demo();
    return 0;
}
