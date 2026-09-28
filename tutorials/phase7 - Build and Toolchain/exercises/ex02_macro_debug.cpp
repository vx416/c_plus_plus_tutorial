/**
 * 練習 2: Macro Debug
 *
 * 比較 macro 和 constexpr function。
 *
 * 重點練習：
 *   - macro 會重複展開參數
 *   - function 只 evaluate 參數一次
 */

#include <cassert>
#include <iostream>
using namespace std;

#define BAD_RUN_TWICE(fn) \
    do {                  \
        (fn)();           \
        (fn)();           \
    } while (false)

constexpr int good_double(int x) {
    return x + x;
}

int main() {
    int a = 3;
    assert(good_double(a) == 6);

    int x = 1;
    auto next_x = [&] {
        ++x;
    };
    BAD_RUN_TWICE(next_x);
    assert(x == 3);

    int y = 1;
    int function_result = good_double(++y);
    assert(function_result == 4);
    assert(y == 2);

    cout << "ex02 passed!" << endl;
    return 0;
}

#undef BAD_RUN_TWICE
