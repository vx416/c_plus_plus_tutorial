/**
 * Phase 7-11: Unit Testing
 *
 * Unit test 是把小單位行為固定下來，讓你改 code 後能快速知道有沒有壞。
 *
 * 目錄:
 *   1. assert-based test
 *   2. test case 命名
 *   3. arrange / act / assert
 *   4. edge cases
 *   5. test framework 形狀
 */

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

int divide(int a, int b) {
    if (b == 0) {
        throw invalid_argument("division by zero");
    }
    return a / b;
}

// ============================================================
// 1. assert-based test
// ============================================================
// 本章重點：
//   assert 是最小測試工具。
//   真正專案會用 GoogleTest/Catch2，但 assert 很適合理解測試基本形狀。
void assert_demo() {
    // Output:
    // === assert-based test ===
    //   divide(6, 2) passed
    //
    cout << "=== assert-based test ===" << endl;
    assert(divide(6, 2) == 3);
    cout << "  divide(6, 2) passed" << endl << endl;
}

// ============================================================
// 2. test case 命名
// ============================================================
// 本章重點：
//   測試名稱應該描述情境和期待，不只是 test1/test2。
void test_divide_positive_numbers() {
    assert(divide(8, 2) == 4);
}

void naming_demo() {
    // Output:
    // === test naming ===
    //   descriptive test name passed
    //
    cout << "=== test naming ===" << endl;
    test_divide_positive_numbers();
    cout << "  descriptive test name passed" << endl << endl;
}

// ============================================================
// 3. arrange / act / assert
// ============================================================
// 本章重點：
//   Arrange: 準備資料。
//   Act: 執行被測行為。
//   Assert: 驗證結果。
void aaa_demo() {
    // Output:
    // === arrange / act / assert ===
    //   AAA style passed
    //
    cout << "=== arrange / act / assert ===" << endl;
    int a = 9;
    int b = 3;
    int result = divide(a, b);
    assert(result == 3);
    cout << "  AAA style passed" << endl << endl;
}

// ============================================================
// 4. edge cases
// ============================================================
// 本章重點：
//   edge case 是最容易壞的邊界。
//   例如除以零、空容器、最大最小值、重複資料。
void edge_case_demo() {
    // Output:
    // === edge cases ===
    //   division by zero throws
    //
    cout << "=== edge cases ===" << endl;
    bool thrown = false;
    try {
        (void)divide(1, 0);
    } catch (const invalid_argument&) {
        thrown = true;
    }
    assert(thrown);
    cout << "  division by zero throws" << endl << endl;
}

// ============================================================
// 5. test framework 形狀
// ============================================================
// 本章重點：
//   GoogleTest/Catch2 會提供更好的 failure message、fixture、parameterized test。
//   但底層概念仍是：執行一段行為，驗證結果。
void framework_demo() {
    // Output:
    // === test framework shape ===
    //   TEST(SuiteName, CaseName) { EXPECT_EQ(...); }
    //
    // all unit testing demos passed
    cout << "=== test framework shape ===" << endl;
    cout << "  TEST(SuiteName, CaseName) { EXPECT_EQ(...); }" << endl << endl;
}

int main() {
    assert_demo();
    naming_demo();
    aaa_demo();
    edge_case_demo();
    framework_demo();
    cout << "all unit testing demos passed" << endl;
    return 0;
}
