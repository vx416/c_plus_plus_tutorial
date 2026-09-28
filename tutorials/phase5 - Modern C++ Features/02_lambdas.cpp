/**
 * Phase 5-2: Lambda Expressions
 *
 * Lambda 是可以直接寫在使用位置的小函式物件。
 * 它最常出現在 STL algorithm、callback、短小策略邏輯裡。
 *
 * 目錄:
 *   1. 基本 lambda
 *   2. capture by value / reference
 *   3. mutable lambda
 *   4. generic lambda
 *   5. lambda 與 STL algorithms
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. 基本 lambda
// ============================================================
// 本章重點：
//   lambda 語法長這樣：
//     [capture](parameters) { body }
//
//   capture 是從外部拿變數。
//   parameters/body 就像一般函式。
void basic_lambda_demo() {
    // Output:
    // === basic lambda ===
    //   add(2, 3) = 5
    //
    cout << "=== basic lambda ===" << endl;

    auto add = [](int a, int b) {
        return a + b;
    };

    cout << "  add(2, 3) = " << add(2, 3) << endl;
    cout << endl;
}

// ============================================================
// 2. capture by value / reference
// ============================================================
// 本章重點：
//   [x] 代表 copy x 進 lambda。
//   [&x] 代表 reference 到外面的 x。
//
//   by value 比較安全，因為 lambda 不依賴外部變數生命週期。
//   by reference 可以修改外部變數，但要確保外部變數還活著。
void capture_demo() {
    // Output:
    // === capture ===
    //   by_value(1) = 11
    //   by_ref(1)   = 21
    //
    cout << "=== capture ===" << endl;

    int base = 10;
    auto by_value = [base](int n) { return base + n; };
    auto by_ref = [&base](int n) { return base + n; };

    base = 20;
    cout << "  by_value(1) = " << by_value(1) << endl;
    cout << "  by_ref(1)   = " << by_ref(1) << endl;
    cout << endl;
}

// ============================================================
// 3. mutable lambda
// ============================================================
// 本章重點：
//   by value capture 預設不能修改 capture 進來的副本。
//   加上 mutable 後，可以修改 lambda 自己那份副本。
//   注意：修改的是副本，不是外部原變數。
void mutable_demo() {
    // Output:
    // === mutable lambda ===
    //   next = 1, 2
    //   outer counter = 0
    //
    cout << "=== mutable lambda ===" << endl;

    int counter = 0;
    auto next = [counter]() mutable {
        return ++counter;
    };

    cout << "  next = " << next() << ", " << next() << endl;
    cout << "  outer counter = " << counter << endl;
    cout << endl;
}

// ============================================================
// 4. generic lambda
// ============================================================
// 本章重點：
//   lambda 參數可以用 auto，這就是 generic lambda。
//   它背後像是一個有 template operator() 的小物件。
void generic_lambda_demo() {
    // Output:
    // === generic lambda ===
    //   twice(3) = 6
    //   twice(string) = haha
    //
    cout << "=== generic lambda ===" << endl;

    auto twice = [](const auto& value) {
        return value + value;
    };

    cout << "  twice(3) = " << twice(3) << endl;
    cout << "  twice(string) = " << twice(string("ha")) << endl;
    cout << endl;
}

// ============================================================
// 5. lambda 與 STL algorithms
// ============================================================
// 本章重點：
//   lambda 讓 algorithm 的條件直接寫在呼叫點。
//   例如 sort 的排序規則、count_if 的判斷條件，不需要另外命名一個小函式。
void algorithm_lambda_demo() {
    // Output:
    // === lambda with algorithms ===
    //   sorted by length: Bob Alice Carol
    //   long name count = 2
    cout << "=== lambda with algorithms ===" << endl;

    vector<string> names{"Bob", "Alice", "Carol"};
    sort(names.begin(), names.end(), [](const string& a, const string& b) {
        return a.size() < b.size();
    });

    cout << "  sorted by length:";
    for (const string& name : names) {
        cout << ' ' << name;
    }
    cout << endl;

    int long_names = count_if(names.begin(), names.end(), [](const string& name) {
        return name.size() >= 5;
    });
    cout << "  long name count = " << long_names << endl;
    cout << endl;
}

int main() {
    basic_lambda_demo();
    capture_demo();
    mutable_demo();
    generic_lambda_demo();
    algorithm_lambda_demo();
    return 0;
}
