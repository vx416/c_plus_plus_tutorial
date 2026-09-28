/**
 * 練習 1: Min/Max Template
 *
 * 實作幾個 function template：
 *   - my_min: 回傳較小值
 *   - my_max: 回傳較大值
 *   - clamp_value: 把值限制在 [low, high]
 *
 * 重點練習：function template、const reference、型別推導
 */

#include <cassert>
#include <iostream>
#include <string>
using namespace std;

template <typename T>
const T& my_min(const T& a, const T& b) {
    // TODO: 回傳較小的那個值
    // 使用 const T& 的原因：
    //   1. 不複製 string/vector 這類可能較大的物件
    //   2. int 這類小型別也可以正常使用
    //
    // 回傳 const T& 代表回傳的是 a 或 b 本身。
    // 因此不要把 temporary 的結果存成長生命週期 reference。
    return (b < a) ? b : a;
}

template <typename T>
const T& my_max(const T& a, const T& b) {
    // TODO: 回傳較大的那個值
    // 這裡只使用 operator<，不使用 operator>。
    // 很多標準 library 演算法也偏好只要求一種比較操作，減少型別需要實作的介面。
    return (a < b) ? b : a;
}

template <typename T>
const T& clamp_value(const T& value, const T& low, const T& high) {
    // TODO: value < low 回傳 low；high < value 回傳 high；否則回傳 value
    // clamp 的三個參數都必須是同一個 T。
    // clamp_value(1, 0.0, 2.0) 會推導失敗，因為 T 不能同時是 int 和 double。
    if (value < low) return low;
    if (high < value) return high;
    return value;
}

int main() {
    assert(my_min(3, 7) == 3);
    assert(my_max(3, 7) == 7);
    assert(clamp_value(5, 1, 10) == 5);
    assert(clamp_value(-1, 1, 10) == 1);
    assert(clamp_value(99, 1, 10) == 10);

    string a = "apple";
    string b = "banana";
    assert(my_min(a, b) == "apple");
    assert(my_max(a, b) == "banana");
    assert(clamp_value(string("cat"), string("ant"), string("bee")) == "bee");

    cout << "ex01 passed!" << endl;
    return 0;
}
