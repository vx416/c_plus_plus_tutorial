/**
 * Phase 3-2: Class Templates
 *
 * Class template 讓 class 可以抽象化「儲存的型別」。
 * 重要觀念：template 參數可以屬於整個 class，也可以只屬於某個 member function。
 * Box<int> 和 Box<string> 是不同的 class instantiation；
 * 但同一個 object 的 template member function 可以在不同呼叫中推導不同型別。
 *
 * 目錄:
 *   1. 基本 class template
 *   2. Template member function
 *   3. Full specialization
 *   4. Partial specialization
 *   5. Alias template
 */

#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// ============================================================
// 1. 基本 class template
// ============================================================
// 本章重點：
//   class template 是「class 的樣板」。
//   Box<T> 的 T 會影響整個 class：member 變數、constructor、method 回傳型別都能用 T。
//
//   Box<int> 和 Box<string> 是不同型別，不是同一個 Box 裡面塞不同資料而已。
//   也就是說，編譯器會替不同 T 產生不同版本的 Box。
template <typename T>
class Box {
public:
    // 這裡用 pass by value + move：
    //   Box<string> b(s)              -> 先 copy s 成參數 value，再 move value 到 value_
    //   Box<string> b(string{"hi"})   -> 先 move/建構參數 value，再 move value 到 value_
    //
    // std::move(value) 搬的是這個 constructor 裡的區域參數，不是一定搬呼叫端的物件。
    // 因此 lvalue 也能傳進來，而且呼叫端的 lvalue 不會被 move。
    explicit Box(T value) : value_(std::move(value)) {}

    // 回傳 const T& 避免複製 value_，也避免外部直接修改內部狀態。
    const T& get() const { return value_; }

    // set 也採用 pass by value + move，理由同 constructor。
    // 若 T 很小如 int，move 和 copy 幾乎沒有差；若 T 是 string/vector，move 可省資源。
    void set(T value) { value_ = std::move(value); }

private:
    T value_;
};

void box_demo() {
    // Output:
    // === 基本 class template ===
    //   Box<int>: 42
    //   Box<string>: hello
    //
    cout << "=== 基本 class template ===" << endl;

    Box<int> a(42);
    Box<string> b("hello");

    cout << "  Box<int>: " << a.get() << endl;
    cout << "  Box<string>: " << b.get() << endl;
    cout << endl;
}

// ============================================================
// 2. Template member function
// ============================================================
// 本章重點：
//   template member function 是「某個 method 自己是 template」。
//
//   這跟 class-level template 不同：
//     class-level template 的 T：建立 Box<int> 時就固定，整個 class 都用同一個 T。
//     member function template 的 U：每次呼叫 method 時才推導，可以每次不同。
//
//   所以 NumberBox<int> 的 value_ 永遠是 int，
//   但 add(2.5) 可以臨時讓 U=double，回傳結果也可以是 double。
template <typename T>
class NumberBox {
public:
    explicit NumberBox(T value) : value_(value) {}

    // 這是 template member function。
    // T 屬於 NumberBox<T> 這個 class，建立 object 時就固定了。
    // U 屬於 add() 這次呼叫，每次呼叫都可以不同。
    //
    // NumberBox<int> box(10) 中 T 固定是 int：
    //   box.add(2.5)  -> U = double
    //   box.add(3)    -> U = int
    //
    // auto 由 value_ + other 的結果推導；int + double 會得到 double。
    template <typename U>
    auto add(U other) const {
        return value_ + other;
    }

private:
    T value_;
};

void member_template_demo() {
    // Output:
    // === Template member function ===
    //   box.add(2.5) = 12.5
    //
    cout << "=== Template member function ===" << endl;

    NumberBox<int> box(10);
    cout << "  box.add(2.5) = " << box.add(2.5) << endl;
    cout << endl;
}

// ============================================================
// 3. Full specialization
// ============================================================
// 本章重點：
//   Full specialization 是「某一個完整型別組合，整份改寫」。
//
//   primary template 是預設版本：
//     TypeName<T> -> unknown
//
//   full specialization 是指定精確型別：
//     TypeName<int>    -> int
//     TypeName<string> -> string
//
//   它不是在原本 struct 裡改一個 function，而是 TypeName<int> 這個版本整份換成新定義。
//   所以 full specialization 常用於「某個特定型別需要完全不同處理」。
template <typename T>
struct TypeName {
    // primary template：所有沒有特別指定的型別都會走這個版本。
    static string name() { return "unknown"; }
};

template <>
struct TypeName<int> {
    // full specialization：T 完全指定成 int。
    // TypeName<int> 會整個改用這份定義，不會繼承 primary template 的內容。
    static string name() { return "int"; }
};

template <>
struct TypeName<string> {
    // 另一個 full specialization：T 完全指定成 string。
    static string name() { return "string"; }
};

void full_specialization_demo() {
    // Output:
    // === Full specialization ===
    //   TypeName<double>: unknown
    //   TypeName<int>: int
    //   TypeName<string>: string
    //
    cout << "=== Full specialization ===" << endl;

    cout << "  TypeName<double>: " << TypeName<double>::name() << endl;
    cout << "  TypeName<int>: " << TypeName<int>::name() << endl;
    cout << "  TypeName<string>: " << TypeName<string>::name() << endl;
    cout << endl;
}

// ============================================================
// 4. Partial specialization
// ============================================================
// 本章重點：
//   Partial specialization 是「不是指定單一完整型別，而是指定一種型別形狀」。
//
//   IsPointer<T> 是預設版本：任何型別先假設不是 pointer。
//   IsPointer<T*> 是偏特化版本：只要型別長得像某個 T 的 pointer，就套用這份。
//
//   對照：
//     IsPointer<int>     -> 不符合 T*，走 primary template，value=false
//     IsPointer<int*>    -> 符合 T*，T=int，走 partial specialization，value=true
//     IsPointer<double*> -> 符合 T*，T=double，走 partial specialization，value=true
//
//   Full specialization 是「只認一個精確型別」。
//   Partial specialization 是「認一群符合形狀的型別」。
template <typename T>
struct IsPointer {
    // primary template：預設任何 T 都不是 pointer。
    static constexpr bool value = false;
};

template <typename T>
struct IsPointer<T*> {
    // partial specialization：只指定「形狀」是 T*，
    // 但 T 本身仍保留為 template 參數，例如 int*、double* 都符合。
    static constexpr bool value = true;
};

void partial_specialization_demo() {
    // Output:
    // === Partial specialization ===
    //   IsPointer<int>::value = false
    //   IsPointer<int*>::value = true
    //
    cout << "=== Partial specialization ===" << endl;

    cout << boolalpha;
    cout << "  IsPointer<int>::value = " << IsPointer<int>::value << endl;
    cout << "  IsPointer<int*>::value = " << IsPointer<int*>::value << endl;
    cout << noboolalpha << endl;
}

// ============================================================
// 5. Alias template
// ============================================================
// 本章重點：
//   Alias template 是「泛型型別別名」。
//   它不建立新型別，只是讓很長的型別名稱比較好讀。
//
//   Matrix<int> 只是 vector<vector<int>> 的另一個名字。
//   如果函式需要 vector<vector<int>>，傳 Matrix<int> 也一樣，因為它們是同一個型別。
template <typename T>
using Matrix = vector<vector<T>>;
// Alias template 不是新型別，只是型別名稱的泛型別名。
// Matrix<int> 等價於 vector<vector<int>>。

void alias_template_demo() {
    // Output:
    // === Alias template ===
    //   grid[1][0] = 3
    cout << "=== Alias template ===" << endl;

    Matrix<int> grid{{1, 2}, {3, 4}};
    cout << "  grid[1][0] = " << grid[1][0] << endl;
    cout << endl;
}

int main() {
    box_demo();
    member_template_demo();
    full_specialization_demo();
    partial_specialization_demo();
    alias_template_demo();
    return 0;
}
