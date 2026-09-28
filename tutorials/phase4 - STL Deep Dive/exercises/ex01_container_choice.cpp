/**
 * 練習 1: Container Choice
 *
 * 統計一串 words 的出現次數，並依照字典序輸出。
 *
 * 重點練習：
 *   - unordered_map 適合快速累加 count
 *   - map 適合需要排序輸出的 key-value
 */

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

map<string, int> count_words_sorted(const vector<string>& words) {
    // TODO: 先用 unordered_map 統計，再轉成 map 取得排序輸出。
    // unordered_map 不保證順序，但平均查找/更新很快。
    // map 會依 key 排序，適合最後輸出穩定結果。
    unordered_map<string, int> counts;
    for (const string& word : words) {
        ++counts[word];
    }

    map<string, int> sorted;
    for (const auto& [word, count] : counts) {
        sorted[word] = count;
    }
    return sorted;
}

int main() {
    vector<string> words{"cpp", "stl", "cpp", "map", "stl", "cpp"};
    map<string, int> result = count_words_sorted(words);

    assert(result.size() == 3);
    assert(result["cpp"] == 3);
    assert(result["map"] == 1);
    assert(result["stl"] == 2);

    auto it = result.begin();
    assert(it->first == "cpp");
    ++it;
    assert(it->first == "map");
    ++it;
    assert(it->first == "stl");

    cout << "ex01 passed!" << endl;
    return 0;
}
