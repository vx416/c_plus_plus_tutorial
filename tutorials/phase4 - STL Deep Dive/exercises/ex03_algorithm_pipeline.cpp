/**
 * 練習 3: Algorithm Pipeline
 *
 * 給一串整數：
 *   1. 保留偶數
 *   2. 每個偶數平方
 *   3. 回傳總和
 *
 * 重點練習：
 *   - copy_if
 *   - transform
 *   - accumulate
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>
using namespace std;

int sum_even_squares(const vector<int>& values) {
    // TODO: 不手寫一個大 for，把三個意圖拆給 STL algorithms。
    vector<int> evens;
    copy_if(values.begin(), values.end(), back_inserter(evens), [](int n) {
        return n % 2 == 0;
    });

    vector<int> squares(evens.size());
    transform(evens.begin(), evens.end(), squares.begin(), [](int n) {
        return n * n;
    });

    return accumulate(squares.begin(), squares.end(), 0);
}

int main() {
    assert(sum_even_squares({1, 2, 3, 4, 5, 6}) == 56);
    assert(sum_even_squares({1, 3, 5}) == 0);
    assert(sum_even_squares({2}) == 4);

    cout << "ex03 passed!" << endl;
    return 0;
}
