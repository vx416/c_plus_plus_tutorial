/**
 * Phase 4-2: Iterators
 *
 * Iterator 是 STL 的核心橋樑。
 * Container 負責存資料；algorithm 不直接知道 container 細節，只透過 iterator 走訪。
 *
 * 目錄:
 *   1. begin/end 是半開區間
 *   2. iterator category
 *   3. const_iterator
 *   4. custom iterator
 *   5. invalidation 實務規則
 */

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. begin/end 是半開區間
// ============================================================
// 本章重點：
//   STL algorithm 通常吃 [begin, end)。
//   左邊 begin 指向第一個元素；右邊 end 指向「最後一個元素的下一格」。
//   end 不是可解參考的元素，它只是停止標記。
void range_demo() {
    // Output:
    // === begin/end range ===
    //   values: 1 2 3
    //
    cout << "=== begin/end range ===" << endl;

    vector<int> nums{1, 2, 3};
    cout << "  values:";
    for (auto it = nums.begin(); it != nums.end(); ++it) {
        cout << ' ' << *it;
    }
    cout << endl << endl;
}

// ============================================================
// 2. iterator category
// ============================================================
// 本章重點：
//   iterator 有能力等級。
//   vector iterator 像 pointer，可以 it + 2，所以是 random access。
//   list iterator 只能一步一步走，不能 it + 2。
//
//   algorithm 會要求特定 iterator 能力。
//   std::sort 需要 random access，所以可排序 vector，但不能直接排序 list。
void category_demo() {
    // Output:
    // === iterator category ===
    //   sorted vector: 1 2 3 4
    //   list has its own sort(): 1 2 3 4
    //
    cout << "=== iterator category ===" << endl;

    vector<int> nums{4, 1, 3, 2};
    sort(nums.begin(), nums.end());
    cout << "  sorted vector:";
    for (int n : nums) {
        cout << ' ' << n;
    }
    cout << endl;

    list<int> linked{4, 1, 3, 2};
    linked.sort();
    cout << "  list has its own sort():";
    for (int n : linked) {
        cout << ' ' << n;
    }
    cout << endl << endl;
}

// ============================================================
// 3. const_iterator
// ============================================================
// 本章重點：
//   const_iterator 代表「可以走訪，但不能透過 iterator 修改元素」。
//   如果函式只讀 container，使用 const reference 和 const_iterator 可以讓意圖更明確。
int sum_read_only(const vector<int>& values) {
    int total = 0;
    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        total += *it;
        // *it = 0; // 編譯錯誤：const_iterator 不能修改元素
    }
    return total;
}

void const_iterator_demo() {
    // Output:
    // === const_iterator ===
    //   sum = 6
    //
    cout << "=== const_iterator ===" << endl;
    vector<int> nums{1, 2, 3};
    cout << "  sum = " << sum_read_only(nums) << endl;
    cout << endl;
}

// ============================================================
// 4. custom iterator
// ============================================================
// 本章重點：
//   自訂 iterator 的目的，是讓自己的型別也能被 range-for 和 STL algorithm 使用。
//   最小可用 iterator 通常要提供：
//     operator*
//     operator++
//     operator!= 或 operator==
//
//   這裡做一個 IntRange，讓 for (int n : IntRange{1, 5}) 可以印 1 2 3 4。
class IntRange {
public:
    class Iterator {
    public:
        using iterator_category = input_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        using pointer = const int*;
        using reference = int;

        explicit Iterator(int current) : current_(current) {}

        int operator*() const { return current_; }

        Iterator& operator++() {
            ++current_;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current_ != other.current_;
        }

    private:
        int current_;
    };

    IntRange(int begin, int end) : begin_(begin), end_(end) {}

    Iterator begin() const { return Iterator(begin_); }
    Iterator end() const { return Iterator(end_); }

private:
    int begin_;
    int end_;
};

void custom_iterator_demo() {
    // Output:
    // === custom iterator ===
    //   IntRange{1, 5}: 1 2 3 4
    //
    cout << "=== custom iterator ===" << endl;

    cout << "  IntRange{1, 5}:";
    for (int n : IntRange{1, 5}) {
        cout << ' ' << n;
    }
    cout << endl << endl;
}

// ============================================================
// 5. invalidation 實務規則
// ============================================================
// 本章重點：
//   iterator invalidation 要查 container 規則，不能靠猜。
//   簡化記法：
//     vector 擴容會讓所有 iterator/reference 失效。
//     map/list 插入通常不影響其他元素 iterator。
//     erase 會讓被刪掉的那個元素 iterator 失效。
void invalidation_rules_demo() {
    // Output:
    // === invalidation rules ===
    //   vector reallocation invalidates iterators
    //   erase invalidates erased element iterator
    //   when unsure, reacquire iterators after mutation
    cout << "=== invalidation rules ===" << endl;
    cout << "  vector reallocation invalidates iterators" << endl;
    cout << "  erase invalidates erased element iterator" << endl;
    cout << "  when unsure, reacquire iterators after mutation" << endl;
    cout << endl;
}

int main() {
    range_demo();
    category_demo();
    const_iterator_demo();
    custom_iterator_demo();
    invalidation_rules_demo();
    return 0;
}
