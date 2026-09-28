/**
 * 練習 5: Assert Tests
 *
 * 寫一組最小 assert-based tests。
 *
 * 重點練習：
 *   - 測試正常情境
 *   - 測試邊界情境
 *   - 測試 exception
 */

#include <cassert>
#include <iostream>
#include <stdexcept>
using namespace std;

int safe_divide(int a, int b) {
    if (b == 0) {
        throw invalid_argument("division by zero");
    }
    return a / b;
}

void test_divide_normal_case() {
    assert(safe_divide(8, 2) == 4);
}

void test_divide_negative_case() {
    assert(safe_divide(-9, 3) == -3);
}

void test_divide_by_zero_throws() {
    bool thrown = false;
    try {
        (void)safe_divide(1, 0);
    } catch (const invalid_argument&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_divide_normal_case();
    test_divide_negative_case();
    test_divide_by_zero_throws();

    cout << "ex05 passed!" << endl;
    return 0;
}
