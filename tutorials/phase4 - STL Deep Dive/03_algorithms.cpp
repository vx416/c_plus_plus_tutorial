/**
 * Phase 4-3: Algorithms
 *
 * STL algorithm 是「操作範圍」的工具。
 * 好處是把常見邏輯交給標準函式，程式更短，也比較不容易寫錯迴圈邊界。
 *
 * 目錄:
 *   1. find / count
 *   2. sort / stable_sort
 *   3. transform
 *   4. accumulate
 *   5. C++20 ranges
 */

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>
using namespace std;

struct User {
    string name;
    int score;
};

// ============================================================
// 1. find / count
// ============================================================
// 本章重點：
//   find/count 是最基本的查找 algorithm。
//   find 回傳 iterator；如果找不到，回傳 end。
//   count 回傳出現次數。
void find_count_demo() {
    // Output:
    // === find / count ===
    //   found: stl
    //   count(stl) = 2
    //
    cout << "=== find / count ===" << endl;

    vector<string> words{"cpp", "stl", "template", "stl"};
    auto it = find(words.begin(), words.end(), "stl");
    if (it != words.end()) {
        cout << "  found: " << *it << endl;
    }

    cout << "  count(stl) = " << count(words.begin(), words.end(), "stl") << endl;
    cout << endl;
}

// ============================================================
// 2. sort / stable_sort
// ============================================================
// 本章重點：
//   sort 需要 random access iterator，所以常搭配 vector。
//   自訂排序時傳 comparator，回傳 true 代表左邊應該排在右邊前面。
//   stable_sort 會保留「排序 key 相同的元素」原本相對順序。
void sort_demo() {
    // Output:
    // === sort ===
    //   sorted by score desc: Alice=90 Carol=90 Bob=75
    //
    cout << "=== sort ===" << endl;

    vector<User> users{{"Alice", 90}, {"Bob", 75}, {"Carol", 90}};
    stable_sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.score > b.score;
    });

    cout << "  sorted by score desc:";
    for (const User& user : users) {
        cout << ' ' << user.name << '=' << user.score;
    }
    cout << endl << endl;
}

// ============================================================
// 3. transform
// ============================================================
// 本章重點：
//   transform 用來把一段資料轉成另一段資料。
//   它比手寫 for 迴圈更明確表達：「我正在做 mapping」。
void transform_demo() {
    // Output:
    // === transform ===
    //   squared: 1 4 9
    //
    cout << "=== transform ===" << endl;

    vector<int> nums{1, 2, 3};
    vector<int> squared;
    squared.resize(nums.size());

    transform(nums.begin(), nums.end(), squared.begin(), [](int n) {
        return n * n;
    });

    cout << "  squared:";
    for (int n : squared) {
        cout << ' ' << n;
    }
    cout << endl << endl;
}

// ============================================================
// 4. accumulate
// ============================================================
// 本章重點：
//   accumulate 是「把一段資料折疊成一個結果」。
//   最常見是加總，也可以累積成字串、物件、統計資料。
//
//   初始值型別很重要：
//     accumulate(v.begin(), v.end(), 0)   -> int 加總
//     accumulate(v.begin(), v.end(), 0.0) -> double 加總
void accumulate_demo() {
    // Output:
    // === accumulate ===
    //   total = 10
    //   joined = 1,2,3,4
    //
    cout << "=== accumulate ===" << endl;

    vector<int> nums{1, 2, 3, 4};
    int total = accumulate(nums.begin(), nums.end(), 0);
    cout << "  total = " << total << endl;

    string joined = accumulate(next(nums.begin()), nums.end(), to_string(nums.front()),
                               [](string acc, int n) {
                                   return acc + "," + to_string(n);
                               });
    cout << "  joined = " << joined << endl;
    cout << endl;
}

// ============================================================
// 5. C++20 ranges
// ============================================================
// 本章重點：
//   ranges 讓 algorithm 更接近「直接操作 container」。
//   std::ranges::sort(nums) 不需要手動寫 begin/end。
//   views 是 lazy pipeline，filter/transform 不會馬上建立新 vector，而是走訪時才計算。
void ranges_demo() {
    // Output:
    // === C++20 ranges ===
    //   even squares: 4 16
    cout << "=== C++20 ranges ===" << endl;

    vector<int> nums{5, 1, 4, 2, 3};
    ranges::sort(nums);

    auto even_squares = nums
        | views::filter([](int n) { return n % 2 == 0; })
        | views::transform([](int n) { return n * n; });

    cout << "  even squares:";
    for (int n : even_squares) {
        cout << ' ' << n;
    }
    cout << endl << endl;
}

int main() {
    find_count_demo();
    sort_demo();
    transform_demo();
    accumulate_demo();
    ranges_demo();
    return 0;
}
