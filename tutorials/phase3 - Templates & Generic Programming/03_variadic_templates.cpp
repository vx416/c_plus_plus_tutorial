/**
 * Phase 3-3: Variadic Templates
 *
 * Variadic template 可以接收任意數量的 template 參數。
 * Args... 代表「一包型別」，args... 代表「一包值」。
 * 它常用在 logging、factory function、wrapper function，
 * 因為這些情境需要把不固定數量的參數轉交給下一層。
 *
 * 目錄:
 *   1. Parameter pack
 *   2. 遞迴展開
 *   3. Fold expression (C++17)
 *   4. Perfect forwarding 基礎
 */

#include <concepts>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

// ============================================================
// 1. Parameter pack
// ============================================================
// 本章重點：
//   parameter pack 是「一包參數」。
//   平常 template <typename T> 只有一個型別 T；
//   template <typename... Args> 則可以有 0 個、1 個或很多個型別。
//
//   Args... 是型別那一包，args... 是值那一包。
//   這種寫法適合做「不知道會收到幾個參數」的工具，例如 log、format、factory。
template <typename... Args>
void count_args(const Args&... args) {
    // Args... 是 type parameter pack。
    // args... 是 function parameter pack。
    // sizeof...(args) 在編譯期取得 pack 裡有幾個元素。
    //
    // 這裡沒有真的使用 args 的值，只示範「任意數量參數」的形狀。
    cout << "  參數數量 = " << sizeof...(args) << endl;
}

void parameter_pack_demo() {
    // Output:
    // === Parameter pack ===
    //   參數數量 = 3
    //   參數數量 = 0
    //
    cout << "=== Parameter pack ===" << endl;

    count_args(1, 2.5, "hello");
    count_args();
    cout << endl;
}

// ============================================================
// 2. 遞迴展開
// ============================================================
// 本章重點：
//   pack 不能直接當成普通陣列用，要透過「展開」把裡面的每個參數拿出來。
//   遞迴展開是舊式但很好理解的方式：
//     先處理第一個 first
//     再把剩下 rest... 丟回同一個函式
//     直到沒有參數，呼叫 base case 停止
void print_recursive() {
    // base case：當 rest... 已經沒有東西時，會呼叫到這個無參數版本。
    cout << endl;
}

template <typename First, typename... Rest>
void print_recursive(const First& first, const Rest&... rest) {
    // 每次拿出第一個參數 first，剩下的仍是一包 rest...。
    // print_recursive(1, "two", 3.0)
    //   -> first = 1,     rest... = "two", 3.0
    //   -> first = "two", rest... = 3.0
    //   -> first = 3.0,   rest... = empty
    cout << first;
    if constexpr (sizeof...(rest) > 0) {
        // if constexpr 是編譯期判斷。
        // 當 rest... 為空，逗號輸出的分支不會被編譯進那個版本。
        cout << ", ";
    }
    print_recursive(rest...);
}

void recursive_demo() {
    // Output:
    // === 遞迴展開 ===
    //   1, two, 3
    //
    cout << "=== 遞迴展開 ===" << endl;

    cout << "  ";
    print_recursive(1, "two", 3.0);
    cout << endl;
}

// ============================================================
// 3. Fold expression
// ============================================================
// 本章重點：
//   fold expression 是 C++17 提供的 pack 展開語法。
//   它讓你不用手寫遞迴，就能把一包參數用某個 operator 串起來。
//
//   常見用途：
//     (args + ...)              -> 把所有 args 相加
//     ((cout << args), ...)     -> 對每個 args 做輸出
//
//   如果你只是要「對每個參數做同一件事」，fold expression 通常比遞迴清楚。
template <typename... Args>
auto sum(Args... args) {
    // Fold expression：把 pack 用 + 串起來。
    // (args + ...) 對 sum(1,2,3,4) 近似展開為 1 + (2 + (3 + 4))。
    // 若 args 為空，這個寫法沒有 identity value，會編譯失敗。
    return (args + ...);
}

template <typename... Args>
void print_fold(const Args&... args) {
    // 逗號 fold 常用來「對 pack 裡每個元素做一件事」。
    // 這裡每個 args 都會依序執行 cout << args << " "。
    ((cout << args << " "), ...);
    cout << endl;
}

template <typename... Args>
string join_space(const Args&... args) {
    ostringstream oss;
    bool first = true;
    // 每次展開都執行：
    //   oss << (first ? "" : " ") << args
    //   first = false
    //
    // 用逗號 operator 把「輸出」和「更新 first」放在同一次展開裡。
    ((oss << (first ? "" : " ") << args, first = false), ...);
    return oss.str();
}

void fold_demo() {
    // Output:
    // === Fold expression ===
    //   sum(1,2,3,4) = 10
    //   print_fold: A 10 B 
    //   join_space: C++ template 3
    //
    cout << "=== Fold expression ===" << endl;

    cout << "  sum(1,2,3,4) = " << sum(1, 2, 3, 4) << endl;
    cout << "  print_fold: ";
    print_fold("A", 10, "B");
    cout << "  join_space: " << join_space("C++", "template", 3) << endl;
    cout << endl;
}

// ============================================================
// 4. Perfect forwarding 基礎
// ============================================================
// 本章重點：
//   forwarding 是「包裝函式收到什麼，就盡量原樣轉交給下一層」。
//   例如 make_owned<Widget>("button", 7) 只是包了一層 make_unique，
//   但我們不希望包裝過程多複製 string，也不希望 lvalue/rvalue 性質被弄錯。
//
//   Args&&... 在 function template 裡是 forwarding reference。
//   搭配 std::forward<Args>(args)...，可以保留每個參數原本是 lvalue 還是 rvalue。
struct Widget {
    Widget(string name, int id) : name(std::move(name)), id(id) {
        cout << "  Widget ctor: " << this->name << ", " << this->id << endl;
    }

    string name;
    int id;
};

template <typename T, typename... Args>
unique_ptr<T> make_owned(Args&&... args) {
    // Args&&... 在 function template 中是 forwarding references。
    // 它可以接 lvalue，也可以接 rvalue。
    //
    // std::forward<Args>(args)... 會把每個參數的原始 value category 保留下來：
    // lvalue 仍轉交成 lvalue，rvalue 仍轉交成 rvalue。
    // 這就是 perfect forwarding 的核心。
    return make_unique<T>(std::forward<Args>(args)...);
}

// ---- 坑 1：只能 forward 一次，而且要放在最後一次使用 ----
// std::forward 之後下一層可能已經把參數搬空，再碰它就是 use-after-move。
// 迴圈裡 forward 是最常見的錯法。
void consume(string s) {
    cout << "  consume: [" << s << "]" << endl;
}

template <typename T>
void forward_twice(T&& value) {
    consume(std::forward<T>(value));
    consume(std::forward<T>(value));   // 第二次收到的是被搬空的殼（moved-from 狀態，內容不保證）
}

// ---- 坑 2：forwarding reference 的 constructor 會搶走 copy constructor ----
// 對 non-const lvalue 的 Greedy，T&& 推導成 Greedy& 是完全匹配，
// 而 const Greedy& 需要多加一層 const，排序輸了。結果複製一個 Greedy 會走進 template。
struct Greedy {
    Greedy() = default;
    Greedy(const Greedy&) { cout << "  Greedy copy ctor" << endl; }

    template <typename T>
    explicit Greedy(T&&) { cout << "  Greedy template ctor" << endl; }
};

// 修法：用 concept 把「T 就是自己」的情況排除掉（concepts 在本 phase 第 5 章）。
struct Fixed {
    Fixed() = default;
    Fixed(const Fixed&) { cout << "  Fixed copy ctor" << endl; }

    template <typename T>
        requires (!same_as<remove_cvref_t<T>, Fixed>)
    explicit Fixed(T&&) { cout << "  Fixed template ctor" << endl; }
};

void forwarding_demo() {
    // Output:
    // === Perfect forwarding 基礎 ===
    //   Widget ctor: button, 7
    //   widget->name = button
    //   [forward twice]
    //   consume: [hello]
    //   consume: []
    //   [greedy overload]
    //   Greedy template ctor
    //   Fixed copy ctor
    cout << "=== Perfect forwarding 基礎 ===" << endl;

    auto widget = make_owned<Widget>("button", 7);
    cout << "  widget->name = " << widget->name << endl;

    cout << "  [forward twice]" << endl;
    forward_twice(string("hello"));

    cout << "  [greedy overload]" << endl;
    Greedy g;
    Greedy g_copy(g);      // 想複製，卻走進 template ctor
    Fixed f;
    Fixed f_copy(f);       // 正確選到 copy ctor
    (void)g_copy; (void)f_copy;
    cout << endl;
}

int main() {
    parameter_pack_demo();
    recursive_demo();
    fold_demo();
    forwarding_demo();
    return 0;
}
