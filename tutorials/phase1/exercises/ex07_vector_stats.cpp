/**
 * 練習 7: Vector Statistics
 *
 * 實作幾個 vector 操作函式，練習 std::vector 和 iterator：
 *   - mean:       計算平均值
 *   - remove_if:  移除所有符合條件的元素（原地修改）
 *   - flatten:    把 2D vector 攤平成 1D
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
using namespace std;

double mean(const vector<int>& v) {
    // TODO: 回傳平均值
    // 限制：vector 保證非空
    return 0.0; // placeholder
}

void remove_negatives(vector<int>& v) {
    // TODO: 移除 v 中所有負數（原地修改）
    // 提示：可以用 erase + remove_if，或自己用 iterator 處理
}

vector<int> flatten(const vector<vector<int>>& matrix) {
    // TODO: 把 2D vector 攤平成 1D
    // 例如 {{1,2}, {3}, {4,5,6}} → {1,2,3,4,5,6}
    return {}; // placeholder
}

int main() {
    // --- mean ---
    assert(mean({1, 2, 3, 4, 5}) == 3.0);
    assert(mean({10}) == 10.0);
    assert(abs(mean({1, 2}) - 1.5) < 1e-9);

    // --- remove_negatives ---
    vector<int> v1 = {3, -1, 4, -5, 2, -3};
    remove_negatives(v1);
    assert(v1 == (vector<int>{3, 4, 2}));

    vector<int> v2 = {-1, -2, -3};
    remove_negatives(v2);
    assert(v2.empty());

    vector<int> v3 = {1, 2, 3};
    remove_negatives(v3);
    assert(v3 == (vector<int>{1, 2, 3}));

    // --- flatten ---
    assert(flatten({{1, 2}, {3}, {4, 5, 6}}) == (vector<int>{1, 2, 3, 4, 5, 6}));
    assert(flatten({}) == (vector<int>{}));
    assert(flatten({{}, {1}, {}}) == (vector<int>{1}));

    cout << "ex07 passed!" << endl;
    return 0;
}
