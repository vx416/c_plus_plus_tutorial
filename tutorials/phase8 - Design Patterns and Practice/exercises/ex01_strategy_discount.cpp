/**
 * 練習 1: Strategy Discount
 *
 * 用 strategy 表示不同折扣演算法。
 *
 * 重點練習：
 *   - algorithm varies -> Strategy
 *   - 呼叫端不寫 if/else 判斷折扣
 */

#include <cassert>
#include <functional>
#include <iostream>
using namespace std;

using DiscountStrategy = function<int(int)>;

int checkout(int cents, const DiscountStrategy& discount) {
    return discount(cents);
}

int main() {
    DiscountStrategy no_discount = [](int cents) {
        return cents;
    };
    DiscountStrategy ten_percent_off = [](int cents) {
        return cents * 90 / 100;
    };

    assert(checkout(1000, no_discount) == 1000);
    assert(checkout(1000, ten_percent_off) == 900);

    cout << "ex01 passed!" << endl;
    return 0;
}
