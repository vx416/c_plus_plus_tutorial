/**
 * 練習 1: Parallel Sum
 *
 * 把 vector 分成兩半，用兩個 thread 分別加總，再合併結果。
 *
 * 重點練習：
 *   - 建立 thread
 *   - join 等待結果
 *   - 每個 thread 寫自己的局部結果，避免共享寫入
 */

#include <cassert>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
using namespace std;

long long parallel_sum(const vector<int>& values) {
    long long left = 0;
    long long right = 0;
    size_t mid = values.size() / 2;

    thread t1([&] {
        left = accumulate(values.begin(), values.begin() + static_cast<ptrdiff_t>(mid), 0LL);
    });
    thread t2([&] {
        right = accumulate(values.begin() + static_cast<ptrdiff_t>(mid), values.end(), 0LL);
    });

    t1.join();
    t2.join();
    return left + right;
}

int main() {
    vector<int> values{1, 2, 3, 4, 5};
    assert(parallel_sum(values) == 15);

    cout << "ex01 passed!" << endl;
    return 0;
}
