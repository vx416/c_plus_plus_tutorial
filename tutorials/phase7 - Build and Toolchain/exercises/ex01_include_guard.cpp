/**
 * 練習 1: Include Guard
 *
 * 用字串示範 header include guard 的標準形狀。
 *
 * 重點練習：
 *   - header 可能被重複 include
 *   - include guard 防止重複定義
 */

#include <cassert>
#include <iostream>
#include <string>
using namespace std;

string include_guard_for(string header_name) {
    // 真實專案會根據路徑和檔名建立唯一 macro 名稱。
    // 這裡用固定結果示範形狀。
    return "#ifndef " + header_name + "\n#define " + header_name + "\n#endif";
}

int main() {
    string guard = include_guard_for("MY_HEADER_HPP");
    assert(guard.find("#ifndef MY_HEADER_HPP") != string::npos);
    assert(guard.find("#define MY_HEADER_HPP") != string::npos);
    assert(guard.find("#endif") != string::npos);

    cout << "ex01 passed!" << endl;
    return 0;
}
