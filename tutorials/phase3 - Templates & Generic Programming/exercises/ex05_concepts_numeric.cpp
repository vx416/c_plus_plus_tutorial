/**
 * 練習 5: Numeric Concepts
 *
 * 實作只接受數值型別的工具：
 *   - Numeric concept: integral 或 floating_point
 *   - average: 回傳平均值
 *   - distance_abs: 回傳兩數距離
 *
 * 重點練習：C++20 concepts、requires、型別限制
 */

#include <cassert>
#include <cmath>
#include <concepts>
#include <iostream>
#include <vector>
using namespace std;

template <typename T>
concept Numeric = integral<T> || floating_point<T>;
// Numeric 是自訂 concept。
// 注意 bool 也符合 integral<T>，所以目前 Numeric<bool> 也是 true。
// 若這不是你要的語意，可以寫成：
//   concept Numeric = (integral<T> && !same_as<T, bool>) || floating_point<T>;

template <Numeric T>
double average(const vector<T>& values) {
    // TODO: 計算平均，values 保證非空
    // 回傳 double，避免 vector<int>{1, 2} 的平均被整數除法截斷。
    // static_cast<double> 是明確表示「這裡要轉成浮點數再累加」。
    double total = 0.0;
    for (T value : values) {
        total += static_cast<double>(value);
    }
    return total / values.size();
}

template <Numeric T>
T distance_abs(T a, T b) {
    // TODO: 回傳絕對距離
    // 對 unsigned 型別要小心：若直接 a - b，較小值減較大值會 underflow。
    // 所以先比較大小，再用較大的減較小的。
    return (a < b) ? (b - a) : (a - b);
}

int main() {
    assert(average(vector<int>{1, 2, 3, 4}) == 2.5);
    assert(abs(average(vector<double>{1.5, 2.5}) - 2.0) < 1e-9);

    assert(distance_abs(10, 3) == 7);
    assert(distance_abs(3, 10) == 7);
    assert(abs(distance_abs(1.5, 4.0) - 2.5) < 1e-9);

    // average(vector<string>{"a", "b"}); // 編譯錯誤：string 不符合 Numeric

    cout << "ex05 passed!" << endl;
    return 0;
}
