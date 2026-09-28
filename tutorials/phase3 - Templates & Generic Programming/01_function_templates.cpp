/**
 * Phase 3-1: Function Templates
 *
 * Function template 讓你寫一次函式，讓編譯器依照呼叫時的型別產生版本。
 * Template 本身不是「已經編譯好的單一函式」；比較像一個產生函式的樣板。
 * 例如 my_max(3, 7) 會產生/使用 T=int 的版本，
 * my_max(2.5, 1.5) 會產生/使用 T=double 的版本。
 *
 * 目錄:
 *   1. 基本 function template
 *   2. Type deduction 如何推導 T
 *   3. 多個 template 參數
 *   4. Non-type template parameter
 *   5. Explicit instantiation / explicit template argument
 */

#include <array>
#include <iostream>
#include <string>
#include <typeinfo>
using namespace std;

// ============================================================
// 1. 基本 function template
// ============================================================
// 本章重點：
//   function template 是「函式樣板」，不是單一函式。
//   你寫一份 my_max，編譯器會依照實際呼叫產生不同版本：
//     my_max(3, 7)                         -> my_max<int>
//     my_max(2.5, 1.5)                     -> my_max<double>
//     my_max(string("cat"), string("dog")) -> my_max<string>
//
//   template <typename T> 的 T 代表「之後由編譯器決定的型別」。
//   呼叫時兩個參數都要能推導成同一個 T，因為 my_max 的宣告是 T my_max(T, T)。
template <typename T>
T my_max(T a, T b) {
    // 這裡要求 T 支援 operator<。
    // 定義 template 時不會先檢查所有可能的 T；
    // 等到真的呼叫 my_max(SomeType{}, SomeType{}) 時，
    // 編譯器才會檢查 SomeType 能不能做 a < b。
    return (a < b) ? b : a;
}

void basic_demo() {
    // Output:
    // === 基本 function template ===
    //   my_max(3, 7) = 7
    //   my_max(2.5, 1.5) = 2.5
    //   my_max(string("cat"), string("dog")) = dog
    //
    cout << "=== 基本 function template ===" << endl;

    cout << "  my_max(3, 7) = " << my_max(3, 7) << endl;
    cout << "  my_max(2.5, 1.5) = " << my_max(2.5, 1.5) << endl;
    cout << "  my_max(string(\"cat\"), string(\"dog\")) = "
         << my_max(string("cat"), string("dog")) << endl;

    cout << endl;
}

// ============================================================
// 2. Type deduction
// ============================================================
// 本章重點：
//   Type deduction 是「編譯器從呼叫參數反推 T 是什麼」。
//   你通常不用手動寫 my_max<int>(3, 7)，因為編譯器看到 3 和 7 就知道 T=int。
//
//   但推導規則會受到參數寫法影響：
//     T value       -> 傳值，會複製，top-level const 通常被拿掉
//     const T&      -> 傳 reference，不複製，外部物件不能透過此參數被修改
//
//   top-level const 指的是 const int cx 這種「變數本身不能被改」的 const。
//   複製成一個新的 value 後，新物件是否 const 不需要沿用，所以 T 會是 int。
template <typename T>
void show_type(T value) {
    // 傳值參數會複製一份 value。
    // 若呼叫端傳 const int，這裡的 T 仍通常推導成 int，
    // 因為 top-level const 對「複製出來的新物件」沒有意義。
    cout << "  T = " << typeid(T).name() << ", value = " << value << endl;
}

template <typename T>
void show_ref_type(const T& value) {
    // const T& 不會複製物件，而且保留「不能修改」的語意。
    // 注意：T 本身仍通常是 int；const 是參數型別 const T& 的一部分。
    cout << "  const T& 接收，T = " << typeid(T).name()
         << ", value = " << value << endl;
}

void deduction_demo() {
    // Output:
    // === Type deduction ===
    //   T = i, value = 42
    //   T = i, value = 100
    //   const T& 接收，T = i, value = 100
    //   my_max<double>(1, 2.5) = 2.5
    //
    cout << "=== Type deduction ===" << endl;

    int x = 42;
    const int cx = 100;

    show_type(x);        // T = int，傳值會複製
    show_type(cx);       // top-level const 被拿掉
    show_ref_type(cx);   // T 仍是 int，但參數保留 const reference

    // my_max(1, 2.5);   // 編譯錯誤：T 不能同時是 int 和 double
    cout << "  my_max<double>(1, 2.5) = " << my_max<double>(1, 2.5) << endl;

    cout << endl;
}

// ============================================================
// 3. 多個 template 參數
// ============================================================
// 本章重點：
//   一個 template 可以有多個型別參數。
//   A 和 B 各自推導，所以 add(1, 2.5) 可以是 A=int、B=double。
//
//   這不表示 A/B 任意組合都能加。
//   template 定義可以先存在，真正用某組型別呼叫時，
//   編譯器才檢查 a + b 對那組型別是否合法。
template <typename A, typename B>
auto add(A a, B b) {
    // auto return type 會由 return expression 推導。
    // add(1, 2.5) 中 a + b 的型別是 double，所以回傳 double。
    //
    // 如果 A 和 B 沒有合法的 operator+，template 定義本身仍可存在；
    // 但呼叫 add(a, b) 讓編譯器 instantiate 該組型別時會編譯失敗。
    return a + b;
}

void multiple_type_demo() {
    // Output:
    // === 多個 template 參數 ===
    //   add(1, 2.5) = 3.5
    //   add(string("hi"), string("!")) = hi!
    //
    cout << "=== 多個 template 參數 ===" << endl;

    cout << "  add(1, 2.5) = " << add(1, 2.5) << endl;
    cout << "  add(string(\"hi\"), string(\"!\")) = "
         << add(string("hi"), string("!")) << endl;

    cout << endl;
}

// ============================================================
// 4. Non-type template parameter
// ============================================================
// 本章重點：
//   template 參數不一定只能是型別，也可以是編譯期常數。
//   這種參數叫 non-type template parameter。
//
//   size_t N 代表陣列大小在編譯期就知道。
//   因此 array<int, 4> 的型別本身就包含「容量 4」，
//   和 array<int, 5> 是不同型別。
template <typename T, size_t N>
T sum_array(const array<T, N>& values) {
    // N 是 non-type template parameter：它不是型別，而是編譯期常數。
    // array<int, 4> 和 array<int, 5> 是不同型別，N 會跟著型別被推導出來。
    T total{};
    for (const T& value : values) {
        total += value;
    }
    return total;
}

template <size_t N>
void print_size(const int (&)[N]) {
    // 參數型別是「reference to C array」。
    // 如果寫成 int*，array 會 decay 成 pointer，長度資訊 N 就消失了。
    cout << "  C array size = " << N << endl;
}

void non_type_parameter_demo() {
    // Output:
    // === Non-type template parameter ===
    //   sum_array({1,2,3,4}) = 10
    //   C array size = 3
    //
    cout << "=== Non-type template parameter ===" << endl;

    array<int, 4> nums{1, 2, 3, 4};
    cout << "  sum_array({1,2,3,4}) = " << sum_array(nums) << endl;

    int raw[] = {10, 20, 30};
    print_size(raw);

    cout << endl;
}

// ============================================================
// 5. Explicit instantiation
// ============================================================
// 本章重點：
//   一般 template 是「用到哪個型別，才產生哪個版本」。
//   explicit instantiation 是你明確叫編譯器先產生某個版本。
//
//   explicit template argument 則是呼叫時手動指定 T：
//     square<double>(5)
//   表示不要讓編譯器從 5 推導成 int，而是強制用 double 版本。
template <typename T>
T square(T x) {
    return x * x;
}

// 明確要求編譯器產生 int 版本。大型專案有時會把 template 實作集中管理。
// 一般學習階段不常需要 explicit instantiation；知道它是「強制產生某版本」即可。
template int square<int>(int);

void explicit_demo() {
    // Output:
    // === Explicit template argument ===
    //   square(5) = 25
    //   square<double>(5) = 25
    cout << "=== Explicit template argument ===" << endl;

    cout << "  square(5) = " << square(5) << endl;
    cout << "  square<double>(5) = " << square<double>(5) << endl;

    cout << endl;
}

int main() {
    basic_demo();
    deduction_demo();
    multiple_type_demo();
    non_type_parameter_demo();
    explicit_demo();
    return 0;
}
