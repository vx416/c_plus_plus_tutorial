/**
 * 練習 4: auto / decltype(auto)
 *
 * 實作兩個 helper：
 *   - get_copy: 回傳 copy
 *   - get_ref: 保留 reference
 *
 * 重點練習：
 *   - auto 會丟掉 reference
 *   - decltype(auto) 可以保留 expression 的 reference 型別
 */

#include <cassert>
#include <iostream>
using namespace std;

int global_value = 10;

int& source_ref() {
    return global_value;
}

auto get_copy() {
    // TODO: auto 會把 int& 回傳結果變成 int copy。
    return source_ref();
}

decltype(auto) get_ref() {
    // TODO: decltype(auto) 保留 source_ref() 的 int&。
    return source_ref();
}

int main() {
    auto copy = get_copy();
    copy = 20;
    assert(copy == 20);
    assert(global_value == 10);

    decltype(auto) ref = get_ref();
    ref = 30;
    assert(global_value == 30);

    cout << "ex04 passed!" << endl;
    return 0;
}
