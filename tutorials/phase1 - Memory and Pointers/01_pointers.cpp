/**
 * Phase 1-1: Pointers 基礎
 *
 * 指標是 C++ 的核心概念，它儲存的是「記憶體位址」而非值本身。
 *
 * 目錄:
 *   1. 基本宣告與操作（取址 & / 解參考 *）
 *   2. Pointer Arithmetic（指標運算）
 *   3. Null Pointer（nullptr）
 *   4. Pointer to Pointer（多層指標）
 *   5. const 與指標的組合（const int* / int* const / const int* const）
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. 基本宣告與操作
// ============================================================
void basic_pointer() {
    int x = 42;
    int* ptr = &x;  // ptr 儲存 x 的位址

    // Output:
    // === 基本宣告 ===
    // x 的值:      42
    // x 的位址:    <address>
    // ptr 的值:    <address>  (就是 x 的位址)
    // ptr 解參考:  42  (透過位址取得 x 的值)
    // 修改後 x:    100
    //
    cout << "=== 基本宣告 ===" << endl;
    cout << "x 的值:      " << x << endl;
    cout << "x 的位址:    " << &x << endl;
    cout << "ptr 的值:    " << ptr << "  (就是 x 的位址)" << endl;
    cout << "ptr 解參考:  " << *ptr << "  (透過位址取得 x 的值)" << endl;

    // 透過指標修改原始變數
    *ptr = 100;
    cout << "修改後 x:    " << x << endl;
    cout << endl;
}

// ============================================================
// 2. Pointer Arithmetic
// ============================================================
void pointer_arithmetic() {
    int arr[] = {10, 20, 30, 40, 50};
    int* p = arr;  // 陣列名本身就是指向第一個元素的指標

    // Output:
    // === Pointer Arithmetic ===
    // 陣列透過指標遍歷:
    //   *(p + 0) = 10
    //   *(p + 1) = 20
    //   *(p + 2) = 30
    //   *(p + 3) = 40
    //   *(p + 4) = 50
    // last - first = 4 個元素
    //
    cout << "=== Pointer Arithmetic ===" << endl;
    cout << "陣列透過指標遍歷:" << endl;

    for (int i = 0; i < 5; ++i) {
        // p + i 會自動乘以 sizeof(int)，跳到第 i 個元素
        cout << "  *(p + " << i << ") = " << *(p + i) << endl;
    }

    // 兩個指標相減 → 得到元素距離（不是 byte 距離）
    int* first = &arr[0];
    int* last = &arr[4];
    cout << "last - first = " << (last - first) << " 個元素" << endl;
    cout << endl;
}

// ============================================================
// 3. Null Pointer
// ============================================================
void null_pointer() {
    // Output:
    // === Null Pointer ===
    // p 是 null，不能解參考
    // q 指向: 77
    //
    cout << "=== Null Pointer ===" << endl;

    // C++11 之後用 nullptr，不要用 NULL 或 0
    int* p = nullptr;

    if (p == nullptr) {
        cout << "p 是 null，不能解參考" << endl;
    }

    // 常見模式：先檢查再使用
    int value = 77;
    int* q = &value;

    if (q != nullptr) {
        cout << "q 指向: " << *q << endl;
    }
    cout << endl;
}

// ============================================================
// 4. Pointer to Pointer
// ============================================================
void pointer_to_pointer() {
    int x = 5;
    int* p = &x;
    int** pp = &p;  // 指向指標的指標

    // Output:
    // === Pointer to Pointer ===
    // x  = 5
    // *p = 5
    // **pp = 5  (兩層解參考)
    //
    cout << "=== Pointer to Pointer ===" << endl;
    cout << "x  = " << x << endl;
    cout << "*p = " << *p << endl;
    cout << "**pp = " << **pp << "  (兩層解參考)" << endl;
    cout << endl;
}

// ============================================================
// 5. const 與指標的組合
// ============================================================
void const_and_pointer() {
    int x = 10, y = 20;

    // Output:
    // === const 與 pointer ===
    // const int* p1 → *p1 = 20
    // int* const p2 → *p2 = 99
    // const int* const p3 → *p3 = 20
    cout << "=== const 與 pointer ===" << endl;

    // 指向 const 的指標：不能透過指標修改值，但指標本身可以改指向
    const int* p1 = &x;
    // *p1 = 99;  // 編譯錯誤！
    p1 = &y;      // OK
    cout << "const int* p1 → *p1 = " << *p1 << endl;

    // const 指標：指標本身不能改指向，但可以透過它修改值
    int* const p2 = &x;
    *p2 = 99;     // OK
    // p2 = &y;   // 編譯錯誤！
    cout << "int* const p2 → *p2 = " << *p2 << endl;

    // 兩者都 const
    const int* const p3 = &y;
    // *p3 = 99;  // 編譯錯誤！
    // p3 = &x;   // 編譯錯誤！
    cout << "const int* const p3 → *p3 = " << *p3 << endl;
    cout << endl;
}

int main() {
    basic_pointer();
    pointer_arithmetic();
    null_pointer();
    pointer_to_pointer();
    const_and_pointer();

    return 0;
}
