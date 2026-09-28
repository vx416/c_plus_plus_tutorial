/**
 * 練習 2: Iterator Range
 *
 * 實作 StepRange，讓 range-for 可以用固定 step 走訪整數。
 *
 * 重點練習：
 *   - begin/end 半開區間
 *   - operator* / operator++ / operator!=
 *   - 自訂型別支援 range-for
 */

#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

class StepRange {
public:
    class Iterator {
    public:
        Iterator(int current, int step) : current_(current), step_(step) {}

        int operator*() const {
            // TODO: 回傳目前值
            return current_;
        }

        Iterator& operator++() {
            // TODO: 前進一個 step
            current_ += step_;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            // TODO: 半開區間停止條件。
            // 這個練習只處理 positive step，所以 current_ >= end 就停止。
            return current_ < other.current_;
        }

    private:
        int current_;
        int step_;
    };

    StepRange(int begin, int end, int step) : begin_(begin), end_(end), step_(step) {}

    Iterator begin() const { return Iterator(begin_, step_); }
    Iterator end() const { return Iterator(end_, step_); }

private:
    int begin_;
    int end_;
    int step_;
};

int main() {
    vector<int> values;
    for (int n : StepRange{1, 8, 2}) {
        values.push_back(n);
    }

    assert((values == vector<int>{1, 3, 5, 7}));

    cout << "ex02 passed!" << endl;
    return 0;
}
