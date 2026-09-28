/**
 * Phase 1-6: Arrays & std::array / std::vector
 *
 * C++ 有三種「陣列」：
 *   - C-style array (int arr[5])     → stack, 固定大小, 沒有邊界檢查
 *   - std::array<int, 5>             → stack, 固定大小, 有 .size() 和 .at()
 *   - std::vector<int>               → heap, 動態大小, 最常用
 *
 * 目錄:
 *   1. C-style Array 的問題（為什麼不建議用）
 *   2. std::array（固定大小的現代替代）
 *   3. std::vector（動態大小，最常用）
 *   4. Vector 記憶體模型（size vs capacity、reserve）
 *   5. Iterator 基礎（遍歷、sort、find、accumulate、reverse）
 *   6. 三者比較表
 */

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

// ============================================================
// 1. C-style Array 的問題
// ============================================================
void c_style_array() {
    // Output:
    // === C-style Array ===
    //   大小: 5
    //   內容: 10 20 30 40 50 
    //   結論: 除非有特殊理由，否則不建議使用 C-style array
    //
    cout << "=== C-style Array ===" << endl;

    int arr[5] = {10, 20, 30, 40, 50};

    // 問題 1: 傳進函式後會退化成指標，大小資訊遺失
    // void foo(int arr[]) → 其實是 void foo(int* arr)

    // 問題 2: 沒有邊界檢查
    // arr[10] = 999;  // 不會報錯，但這是 undefined behavior

    // 問題 3: 不能直接取得大小（需要用 sizeof trick）
    size_t size = sizeof(arr) / sizeof(arr[0]);  // 只在同一個 scope 有效
    cout << "  大小: " << size << endl;

    cout << "  內容: ";
    for (int i = 0; i < 5; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;

    cout << "  結論: 除非有特殊理由，否則不建議使用 C-style array" << endl;
    cout << endl;
}

// ============================================================
// 2. std::array — 固定大小的現代替代
// ============================================================
void std_array_demo() {
    // Output:
    // === std::array ===
    //   大小: 5
    //   arr.at(2) = 30
    //   內容: 10 20 30 40 50 
    //   特性: stack 上配置, 零額外開銷, 大小 compile-time 固定
    //
    cout << "=== std::array ===" << endl;

    array<int, 5> arr = {10, 20, 30, 40, 50};

    // 有 .size()
    cout << "  大小: " << arr.size() << endl;

    // .at() 有邊界檢查（越界會 throw out_of_range）
    cout << "  arr.at(2) = " << arr.at(2) << endl;

    // 支援 range-based for
    cout << "  內容: ";
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;

    // 在 stack 上，效能和 C array 一樣
    // 大小是 compile-time 固定的
    cout << "  特性: stack 上配置, 零額外開銷, 大小 compile-time 固定" << endl;
    cout << endl;
}

// ============================================================
// 3. std::vector — 動態大小，最常用
// ============================================================
void vector_demo() {
    // Output:
    // === std::vector ===
    //   v1: 10 20 30 
    //   size:     3  (實際元素數)
    //   capacity: 4  (已配置的空間)
    //   v1[0] = 10  (不檢查邊界)
    //   v1.at(0) = 10  (檢查邊界)
    //   v1.front() = 10
    //   v1.back() = 40
    //   pop_back 後 size: 3
    //   clear 後 size: 0, capacity: 4  (capacity 不會縮小)
    //
    cout << "=== std::vector ===" << endl;

    // 建立方式
    vector<int> v1;                    // 空的
    vector<int> v2 = {1, 2, 3};       // initializer list
    vector<int> v3(5, 0);             // 5 個 0

    // push_back: 在尾端加入元素
    v1.push_back(10);
    v1.push_back(20);
    v1.push_back(30);

    cout << "  v1: ";
    for (int val : v1) cout << val << " ";
    cout << endl;

    // size 和 capacity
    cout << "  size:     " << v1.size() << "  (實際元素數)" << endl;
    cout << "  capacity: " << v1.capacity() << "  (已配置的空間)" << endl;

    // emplace_back: 原地建構，比 push_back 少一次複製
    v1.emplace_back(40);

    // 存取
    cout << "  v1[0] = " << v1[0] << "  (不檢查邊界)" << endl;
    cout << "  v1.at(0) = " << v1.at(0) << "  (檢查邊界)" << endl;
    cout << "  v1.front() = " << v1.front() << endl;
    cout << "  v1.back() = " << v1.back() << endl;

    // 刪除
    v1.pop_back();  // 移除最後一個
    cout << "  pop_back 後 size: " << v1.size() << endl;

    // 清空
    v1.clear();
    cout << "  clear 後 size: " << v1.size()
         << ", capacity: " << v1.capacity() << "  (capacity 不會縮小)" << endl;
    cout << endl;
}

// ============================================================
// 4. Vector 的記憶體模型
// ============================================================
void vector_memory() {
    // Output:
    // === Vector 記憶體模型 ===
    //   初始 capacity: 0
    //   push_back(0) → capacity: 0 → 1  (重新配置 + 搬移所有元素!)
    //   push_back(1) → capacity: 1 → 2  (重新配置 + 搬移所有元素!)
    //   push_back(2) → capacity: 2 → 4  (重新配置 + 搬移所有元素!)
    //   push_back(4) → capacity: 4 → 8  (重新配置 + 搬移所有元素!)
    //   push_back(8) → capacity: 8 → 16  (重新配置 + 搬移所有元素!)
    //
    //   觀察: capacity 通常以 2 倍成長
    //   如果預知大小，用 reserve() 避免多次 reallocation:
    //   reserve(100) 後 capacity: 100, size: 0
    //
    cout << "=== Vector 記憶體模型 ===" << endl;

    vector<int> v;
    cout << "  初始 capacity: " << v.capacity() << endl;

    for (int i = 0; i < 10; ++i) {
        size_t old_cap = v.capacity();
        v.push_back(i);
        if (v.capacity() != old_cap) {
            cout << "  push_back(" << i << ") → capacity: "
                 << old_cap << " → " << v.capacity()
                 << "  (重新配置 + 搬移所有元素!)" << endl;
        }
    }

    cout << endl;
    cout << "  觀察: capacity 通常以 2 倍成長" << endl;
    cout << "  如果預知大小，用 reserve() 避免多次 reallocation:" << endl;

    vector<int> v2;
    v2.reserve(100);  // 預先配置空間
    cout << "  reserve(100) 後 capacity: " << v2.capacity()
         << ", size: " << v2.size() << endl;
    cout << endl;
}

// ============================================================
// 5. Iterator 基礎
// ============================================================
void iterator_basics() {
    // Output:
    // === Iterator 基礎 ===
    //   用 iterator 遍歷: 50 30 10 40 20 
    //   排序後: 10 20 30 40 50 
    //   找到 30，位置: index 2
    //   總和: 150
    //   反向: 50 40 30 20 10 
    //
    cout << "=== Iterator 基礎 ===" << endl;

    vector<int> v = {50, 30, 10, 40, 20};

    // iterator 就像一個「抽象化的指標」
    cout << "  用 iterator 遍歷: ";
    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << " ";  // 解參考取值，和指標一樣
    }
    cout << endl;

    // range-based for 其實就是 iterator 的語法糖
    // for (int val : v) 等同於上面的寫法

    // 搭配 algorithm
    sort(v.begin(), v.end());
    cout << "  排序後: ";
    for (int val : v) cout << val << " ";
    cout << endl;

    // find
    auto it = find(v.begin(), v.end(), 30);
    if (it != v.end()) {
        cout << "  找到 30，位置: index " << distance(v.begin(), it) << endl;
    }

    // accumulate（加總）
    int sum = accumulate(v.begin(), v.end(), 0);
    cout << "  總和: " << sum << endl;

    // reverse iterator
    cout << "  反向: ";
    for (auto rit = v.rbegin(); rit != v.rend(); ++rit) {
        cout << *rit << " ";
    }
    cout << endl;
    cout << endl;
}

// ============================================================
// 6. 比較表
// ============================================================
void comparison() {
    // Output:
    // === 比較表 ===
    //   +-----------------+------------+------------+------------+
    //   |                 | C array    | std::array | std::vector|
    //   +-----------------+------------+------------+------------+
    //   | 大小            | 固定       | 固定       | 動態       |
    //   | 配置位置        | stack      | stack      | heap       |
    //   | 邊界檢查        | 無         | .at()      | .at()      |
    //   | .size()         | 無         | 有         | 有         |
    //   | 傳入函式        | 退化成指標 | 保持型別   | 保持型別   |
    //   | 額外開銷        | 無         | 無         | 些微       |
    //   +-----------------+------------+------------+------------+
    //
    //   預設選擇: std::vector
    //   固定小陣列: std::array
    //   C-style array: 只在和 C API 互動時使用
    cout << "=== 比較表 ===" << endl;
    cout << "  +-----------------+------------+------------+------------+" << endl;
    cout << "  |                 | C array    | std::array | std::vector|" << endl;
    cout << "  +-----------------+------------+------------+------------+" << endl;
    cout << "  | 大小            | 固定       | 固定       | 動態       |" << endl;
    cout << "  | 配置位置        | stack      | stack      | heap       |" << endl;
    cout << "  | 邊界檢查        | 無         | .at()      | .at()      |" << endl;
    cout << "  | .size()         | 無         | 有         | 有         |" << endl;
    cout << "  | 傳入函式        | 退化成指標 | 保持型別   | 保持型別   |" << endl;
    cout << "  | 額外開銷        | 無         | 無         | 些微       |" << endl;
    cout << "  +-----------------+------------+------------+------------+" << endl;
    cout << endl;
    cout << "  預設選擇: std::vector" << endl;
    cout << "  固定小陣列: std::array" << endl;
    cout << "  C-style array: 只在和 C API 互動時使用" << endl;
    cout << endl;
}

int main() {
    c_style_array();
    std_array_demo();
    vector_demo();
    vector_memory();
    iterator_basics();
    comparison();

    return 0;
}
