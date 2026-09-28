/**
 * 練習 3: Find Max (Reference 回傳)
 *
 * 實作 find_max，回傳 vector 中最大元素的 reference。
 * 呼叫者可以透過回傳的 reference 直接修改該元素。
 *
 * 限制：vector 保證非空。
 */

#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

int &find_max(vector<int> &v)
{
    // TODO: 回傳最大元素的 reference
    // 提示：不能回傳 local variable 的 reference
    size_t maxIdx = 0;
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i] > v[maxIdx]) {
            maxIdx = i;
        }
    }
    return v[maxIdx]; // placeholder，請修改
}

int main()
{
    vector<int> v1 = {3, 7, 1, 9, 4};
    assert(find_max(v1) == 9);

    // 透過 reference 修改最大值
    find_max(v1) = 0;
    assert(v1[3] == 0); // 原本的 9 被改成 0

    vector<int> v2 = {42};
    assert(find_max(v2) == 42);

    vector<int> v3 = {-5, -1, -10};
    assert(find_max(v3) == -1);

    cout << "ex03 passed!" << endl;
    return 0;
}
