/**
 * Phase 4-1: Containers 總覽
 *
 * STL container 是「存資料的標準工具箱」。
 * 這章不是要背每個 container 的所有 API，而是先學會怎麼選。
 *
 * 目錄:
 *   1. sequence containers: vector / deque / list
 *   2. associative containers: map / set
 *   3. unordered containers: unordered_map / unordered_set
 *   4. iterator invalidation 基礎
 *   5. container 選擇準則
 */

#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

// ============================================================
// 1. sequence containers: vector / deque / list
// ============================================================
// 本章重點：
//   sequence container 會保留元素順序。
//   vector 是最常用的預設選擇：連續記憶體、cache locality 好、尾端 push 很快。
//   deque 適合前後兩端都要 push/pop。
//   list 是雙向 linked list，中間插入刪除便宜，但走訪慢、記憶體分散。
void sequence_demo() {
    // Output:
    // === sequence containers ===
    //   vector back = 4
    //   deque front/back = urgent / render
    //   list values: 10 20 30
    //
    cout << "=== sequence containers ===" << endl;

    vector<int> nums{1, 2, 3};
    nums.push_back(4);
    cout << "  vector back = " << nums.back() << endl;

    deque<string> jobs;
    jobs.push_back("render");
    jobs.push_front("urgent");
    cout << "  deque front/back = " << jobs.front() << " / " << jobs.back() << endl;

    list<int> linked{10, 30};
    auto it = linked.begin();
    ++it;
    linked.insert(it, 20);
    cout << "  list values:";
    for (int value : linked) {
        cout << ' ' << value;
    }
    cout << endl << endl;
}

// ============================================================
// 2. associative containers: map / set
// ============================================================
// 本章重點：
//   associative container 依 key 排序，通常用平衡樹實作。
//   map 存 key -> value；set 只存 key。
//   查找、插入、刪除通常是 O(log n)，而且走訪時會得到排序後的順序。
void associative_demo() {
    // Output:
    // === associative containers ===
    //   map iteration is sorted by key: Alice=95 Bob=80 Carol=88
    //   set removes duplicates and sorts: 1 2 3
    //
    cout << "=== associative containers ===" << endl;

    map<string, int> score_by_name{{"Bob", 80}, {"Alice", 95}, {"Carol", 88}};
    cout << "  map iteration is sorted by key:";
    for (const auto& [name, score] : score_by_name) {
        cout << ' ' << name << '=' << score;
    }
    cout << endl;

    set<int> unique_sorted{3, 1, 3, 2};
    cout << "  set removes duplicates and sorts:";
    for (int value : unique_sorted) {
        cout << ' ' << value;
    }
    cout << endl << endl;
}

// ============================================================
// 3. unordered containers: unordered_map / unordered_set
// ============================================================
// 本章重點：
//   unordered container 用 hash table。
//   它不保證走訪順序，但平均查找很快，通常是 O(1)。
//   如果你只在乎「有沒有這個 key」或「快速查 key」，不在乎排序，就考慮 unordered。
void unordered_demo() {
    // Output:
    // === unordered containers ===
    //   count(red) = 3
    //   has stl = true
    //
    cout << "=== unordered containers ===" << endl;

    unordered_map<string, int> counts;
    for (const string& word : {"red", "blue", "red", "green", "red"}) {
        ++counts[word];
    }
    cout << "  count(red) = " << counts["red"] << endl;

    unordered_set<string> tags{"cpp", "stl", "cpp"};
    cout << "  has stl = " << boolalpha << (tags.count("stl") > 0) << noboolalpha << endl;
    cout << endl;
}

// ============================================================
// 4. iterator invalidation 基礎
// ============================================================
// 本章重點：
//   iterator/reference 不是永遠有效。
//   vector push_back 可能觸發重新配置，把資料搬到新記憶體，舊 iterator 就失效。
//   寫 STL code 時，只要 container 被修改，就要確認手上的 iterator 還能不能用。
void invalidation_demo() {
    // Output:
    // === iterator invalidation ===
    //   iterator still valid after push_back with enough capacity: 1
    //   rule: after modifying a container, re-check iterator validity
    //
    cout << "=== iterator invalidation ===" << endl;

    vector<int> values{1, 2, 3};
    values.reserve(10);
    auto it = values.begin();
    values.push_back(4);
    cout << "  iterator still valid after push_back with enough capacity: " << *it << endl;

    // 如果沒有 reserve，push_back 可能讓 vector 重新配置。
    // 重新配置後，舊 iterator、舊 pointer、舊 reference 都不能再用。
    cout << "  rule: after modifying a container, re-check iterator validity" << endl;
    cout << endl;
}

// ============================================================
// 5. container 選擇準則
// ============================================================
// 本章重點：
//   不要用「看起來資料結構很酷」來選 container。
//   先問資料怎麼被使用：
//     需要依序掃描？vector。
//     需要 key 查找？map/unordered_map。
//     需要排序後走訪？map/set。
//     需要快速存在檢查？unordered_set。
void choice_demo() {
    // Output:
    // === container choice ===
    //   default list of items: vector
    //   sorted key-value table: map
    //   fast lookup by key: unordered_map
    //   unique sorted values: set
    cout << "=== container choice ===" << endl;
    cout << "  default list of items: vector" << endl;
    cout << "  sorted key-value table: map" << endl;
    cout << "  fast lookup by key: unordered_map" << endl;
    cout << "  unique sorted values: set" << endl;
    cout << endl;
}

int main() {
    sequence_demo();
    associative_demo();
    unordered_demo();
    invalidation_demo();
    choice_demo();
    return 0;
}
