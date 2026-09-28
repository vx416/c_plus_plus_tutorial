/**
 * Phase 1-2: References vs Pointers
 *
 * Reference 是變數的「別名」，一旦綁定就不能改指向。
 * 本節比較 pass by value / reference / pointer 的差異與使用時機。
 *
 * 目錄:
 *   1. Reference 基礎（別名概念）
 *   2. Pass by Value（複製品，不影響原始變數）
 *   3. Pass by Reference（直接操作原始變數）
 *   4. Pass by Pointer（透過位址操作）
 *   5. 何時用 reference、何時用 pointer
 *   6. Dangling Reference 警告（要避免的錯誤）
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
// 1. Reference 基礎
// ============================================================
void reference_basics() {
    int x = 42;
    int& ref = x;  // ref 是 x 的別名，不是複製

    // Output:
    // === Reference 基礎 ===
    // x = 42, ref = 42
    // 修改 ref 後 x = 100
    // &x   = <address>
    // &ref = <address>  (同一個位址)
    //
    cout << "=== Reference 基礎 ===" << endl;
    cout << "x = " << x << ", ref = " << ref << endl;

    ref = 100;  // 修改 ref 就是修改 x
    cout << "修改 ref 後 x = " << x << endl;

    // reference 和原變數的位址相同
    cout << "&x   = " << &x << endl;
    cout << "&ref = " << &ref << "  (同一個位址)" << endl;
    cout << endl;
}

// ============================================================
// 2. Pass by Value — 函式拿到的是複製品
// ============================================================
void increment_by_value(int n) {
    n += 1;  // 只改了複製品，原始變數不受影響
}

// ============================================================
// 3. Pass by Reference — 函式直接操作原始變數
// ============================================================
void increment_by_reference(int& n) {
    n += 1;  // 直接修改原始變數
}

// ============================================================
// 4. Pass by Pointer — 函式透過位址操作原始變數
// ============================================================
void increment_by_pointer(int* n) {
    if (n != nullptr) {
        *n += 1;
    }
}

void compare_passing() {
    // Output:
    // === Pass by Value / Reference / Pointer ===
    // pass by value:     a = 10  (沒變)
    // pass by reference: b = 11  (被修改)
    // pass by pointer:   c = 11  (被修改)
    //
    cout << "=== Pass by Value / Reference / Pointer ===" << endl;
    int a = 10, b = 10, c = 10;

    increment_by_value(a);
    cout << "pass by value:     a = " << a << "  (沒變)" << endl;

    increment_by_reference(b);
    cout << "pass by reference: b = " << b << "  (被修改)" << endl;

    increment_by_pointer(&c);
    cout << "pass by pointer:   c = " << c << "  (被修改)" << endl;
    cout << endl;
}

// ============================================================
// 5. 何時用 reference，何時用 pointer？
// ============================================================

// 用 const reference 避免不必要的複製（尤其是大物件）
void print_info(const string& name) {
    cout << "Name: " << name << endl;
    // name 不會被複製，也不能被修改
}

// pointer 可以是 nullptr → 表示「可選參數」
void maybe_update(int* out_result) {
    if (out_result != nullptr) {
        *out_result = 999;
    }
}

void when_to_use_what() {
    // Output:
    // === 選擇指南 ===
    // Reference:
    //   - 不會是 null
    //   - 語法更簡潔 (不需要 * 和 &)
    //   - 適合：函式參數、回傳值的別名
    //
    // Pointer:
    //   - 可以是 nullptr（表示「沒有」）
    //   - 可以重新指向不同物件
    //   - 適合：optional 參數、動態記憶體、資料結構 (linked list)
    //
    // Name: C++ Tutorial
    // result = 999
    //
    cout << "=== 選擇指南 ===" << endl;
    cout << "Reference:" << endl;
    cout << "  - 不會是 null" << endl;
    cout << "  - 語法更簡潔 (不需要 * 和 &)" << endl;
    cout << "  - 適合：函式參數、回傳值的別名" << endl;
    cout << endl;
    cout << "Pointer:" << endl;
    cout << "  - 可以是 nullptr（表示「沒有」）" << endl;
    cout << "  - 可以重新指向不同物件" << endl;
    cout << "  - 適合：optional 參數、動態記憶體、資料結構 (linked list)" << endl;
    cout << endl;

    string name = "C++ Tutorial";
    print_info(name);

    int result;
    maybe_update(&result);
    cout << "result = " << result << endl;

    maybe_update(nullptr);  // 傳 null 表示不需要結果，不會 crash
    cout << endl;
}

// ============================================================
// 6. Dangling Reference（懸垂參考）— 要避免的錯誤
// ============================================================
void dangling_reference_warning() {
    // Output:
    // === Dangling Reference 警告 ===
    // 以下是錯誤示範（已註解掉）：
    //
    //     int& bad() {
    //         int local = 42;
    //         return local;  // 錯！local 在函式結束後就消失了
    //     }
    //     
    // 回傳 local 變數的 reference 會導致 undefined behavior。
    cout << "=== Dangling Reference 警告 ===" << endl;
    cout << "以下是錯誤示範（已註解掉）：" << endl;
    cout << R"(
    int& bad() {
        int local = 42;
        return local;  // 錯！local 在函式結束後就消失了
    }
    )" << endl;
    cout << "回傳 local 變數的 reference 會導致 undefined behavior。" << endl;
    cout << endl;
}

int main() {
    reference_basics();
    compare_passing();
    when_to_use_what();
    dangling_reference_warning();

    return 0;
}
