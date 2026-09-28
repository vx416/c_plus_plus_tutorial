/**
 * Phase 3-4: SFINAE & if constexpr
 *
 * SFINAE: Substitution Failure Is Not An Error.
 * Template 代入型別失敗時，不一定是編譯錯，而是讓 overload set 排除該版本。
 * 這是 C++20 concepts 以前常見的「限制 template 能接受哪些型別」的方法。
 * 新 code 通常優先用 concepts；SFINAE 主要用來讀懂舊 code 和 library 實作。
 *
 * 目錄:
 *   1. type traits
 *   2. enable_if 寫法
 *   3. if constexpr 寫法
 *   4. detection idiom 基礎
 */

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
using namespace std;

// ============================================================
// 1. type traits
// ============================================================
// 本章重點：
//   type traits 是「在編譯期問型別問題」的工具。
//   例如：
//     T 是整數嗎？
//     T 是 pointer 嗎？
//     T 是 class 嗎？
//
//   這些答案不是 runtime 算出來的，而是在編譯期就是 true/false。
//   後面 enable_if、if constexpr、concepts 都常建立在這種型別判斷上。
template <typename T>
void inspect_type() {
    // type traits 是編譯期查詢型別特性的工具。
    // is_integral_v<int> 這類結果是 constexpr bool，可用來選 overload 或分支。
    cout << boolalpha;
    cout << "  is_integral = " << is_integral_v<T> << endl;
    cout << "  is_pointer  = " << is_pointer_v<T> << endl;
    cout << "  is_class    = " << is_class_v<T> << endl;
    cout << noboolalpha;
}

void traits_demo() {
    // Output:
    // === type traits ===
    //   is_integral = true
    //   is_pointer  = false
    //   is_class    = false
    //
    cout << "=== type traits ===" << endl;
    inspect_type<int>();
    cout << endl;
}

// ============================================================
// 2. enable_if 寫法
// ============================================================
// 本章重點：
//   enable_if 是 C++20 concepts 以前常用的 template 限制方式。
//   它的目的不是拿來算值，而是「條件不符合時，讓這個 overload 消失」。
//
//   這裡有兩個 category：
//     category(int)    -> 只保留 integer 版本
//     category(double) -> 只保留 floating 版本
//
//   如果沒有 enable_if，兩個 template 長得太像，編譯器不知道該選哪個。
template <typename T, enable_if_t<is_integral_v<T>, int> = 0>
string category(T) {
    // 只有 is_integral_v<T> 為 true 時，enable_if_t<true, int> 才會變成 int。
    // 若 T 是 double，enable_if_t<false, int> 沒有 type，這個 overload 會被 SFINAE 排除。
    return "integer";
}

template <typename T, enable_if_t<is_floating_point_v<T>, int> = 0>
string category(T) {
    // T 是浮點數時，這個 overload 留在候選集合中。
    // T 是 int 時，這個 overload 被排除，上一個 overload 會被選到。
    return "floating";
}

void enable_if_demo() {
    // Output:
    // === enable_if ===
    //   category(10) = integer
    //   category(1.5) = floating
    //
    cout << "=== enable_if ===" << endl;
    cout << "  category(10) = " << category(10) << endl;
    cout << "  category(1.5) = " << category(1.5) << endl;
    cout << endl;
}

// ============================================================
// 3. if constexpr 寫法
// ============================================================
// 本章重點：
//   if constexpr 是「編譯期 if」。
//   普通 if 的兩邊都要能通過編譯，只是 runtime 決定走哪邊。
//   if constexpr 沒被選到的分支不會被編譯進該版本。
//
//   所以同一個 describe_value<T> 可以針對 int、double、string 寫不同 code，
//   而不用拆成很多 overload。
template <typename T>
string describe_value(const T& value) {
    // if constexpr 和普通 if 不同：條件在編譯期決定。
    // 沒被選到的分支不會被 instantiate，因此可以在不同分支寫只對某些型別合法的 code。
    if constexpr (is_integral_v<T>) {
        return "integer: " + to_string(value);
    } else if constexpr (is_floating_point_v<T>) {
        return "floating: " + to_string(value);
    } else if constexpr (is_same_v<T, string>) {
        return "string: " + value;
    } else {
        return "unknown";
    }
}

void if_constexpr_demo() {
    // Output:
    // === if constexpr ===
    //   integer: 42
    //   floating: 3.140000
    //   string: hello
    //
    cout << "=== if constexpr ===" << endl;
    cout << "  " << describe_value(42) << endl;
    cout << "  " << describe_value(3.14) << endl;
    cout << "  " << describe_value(string("hello")) << endl;
    cout << endl;
}

// ============================================================
// 4. detection idiom 基礎
// ============================================================
// 本章重點：
//   detection idiom 是「偵測某個 expression 對某型別是否合法」。
//   例如這裡要問：
//     T 有沒有 .size() 這個 member function？
//
//   如果有，就印 value.size()。
//   如果沒有，就走 fallback。
//
//   這種技巧常用於舊式泛型 library。C++20 後通常可用 requires/concepts 寫得更直覺。
template <typename, typename = void>
struct HasSize : false_type {};
// primary template：預設認為型別沒有 size()。

template <typename T>
struct HasSize<T, void_t<decltype(declval<T>().size())>> : true_type {};
// specialization：
//   declval<T>().size() 只用在 unevaluated context，不會真的產生物件。
//   如果這個 expression 合法，void_t<...> 變成 void，匹配到 true_type 版本。
//   如果 expression 不合法，代入失敗，回到 primary template 的 false_type。

template <typename T>
void print_size_if_possible(const T& value) {
    // 先用 HasSize<T> 在編譯期判斷，再只編譯合法分支。
    // 若 T=int，value.size() 那個分支不會被編譯，所以不會報錯。
    if constexpr (HasSize<T>::value) {
        cout << "  size = " << value.size() << endl;
    } else {
        cout << "  no size()" << endl;
    }
}

void detection_demo() {
    // Output:
    // === detection idiom ===
    //   size = 3
    //   no size()
    cout << "=== detection idiom ===" << endl;

    vector<int> nums{1, 2, 3};
    print_size_if_possible(nums);
    print_size_if_possible(123);
    cout << endl;
}

int main() {
    traits_demo();
    enable_if_demo();
    if_constexpr_demo();
    detection_demo();
    return 0;
}
