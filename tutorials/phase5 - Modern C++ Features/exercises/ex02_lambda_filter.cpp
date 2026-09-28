/**
 * 練習 2: Lambda Filter
 *
 * 用 lambda 找出長度大於等於 min_len 的字串，並轉成大寫。
 *
 * 重點練習：
 *   - lambda capture
 *   - copy_if
 *   - transform
 */

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string upper_copy(string text) {
    transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return text;
}

vector<string> long_names_upper(const vector<string>& names, size_t min_len) {
    // TODO: 用 [min_len] capture 過濾，再用 transform 轉大寫。
    vector<string> filtered;
    copy_if(names.begin(), names.end(), back_inserter(filtered), [min_len](const string& name) {
        return name.size() >= min_len;
    });

    transform(filtered.begin(), filtered.end(), filtered.begin(), [](const string& name) {
        return upper_copy(name);
    });

    return filtered;
}

int main() {
    vector<string> result = long_names_upper({"bob", "alice", "carol"}, 5);
    assert((result == vector<string>{"ALICE", "CAROL"}));

    cout << "ex02 passed!" << endl;
    return 0;
}
