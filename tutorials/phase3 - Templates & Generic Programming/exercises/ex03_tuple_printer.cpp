/**
 * 練習 3: Variadic Printer
 *
 * 實作接收任意參數數量的字串組合工具：
 *   - join: 用分隔符串接任意數量參數
 *   - count_args: 回傳參數數量
 *
 * 重點練習：variadic template、sizeof...、fold expression
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

template <typename... Args>
size_t count_args(const Args&...) {
    // TODO: 回傳參數數量
    // sizeof...(Args) 看的是型別 pack 的數量。
    // sizeof...(args) 也可以，但這裡沒有替參數 pack 命名。
    return sizeof...(Args);
}

template <typename... Args>
string join(const string& sep, const Args&... args) {
    // TODO: 用 sep 串接所有 args
    ostringstream oss;
    bool first = true;
    // 逗號 fold：
    //   對每個 args 執行一次 oss << ...
    //   然後把 first 設成 false
    //
    // join("-") 沒有 args 時，fold expression 對逗號 operator 有空 pack 規則，
    // 不會輸出任何東西，因此回傳空字串。
    ((oss << (first ? "" : sep) << args, first = false), ...);
    return oss.str();
}

int main() {
    assert(count_args() == 0);
    assert(count_args(1, 2, 3) == 3);
    assert(count_args("a", 1, 2.0, string("b")) == 4);

    assert(join(", ", 1, 2, 3) == "1, 2, 3");
    assert(join(" / ", "C++", "templates", 20) == "C++ / templates / 20");
    assert(join("-") == "");

    cout << "ex03 passed!" << endl;
    return 0;
}
