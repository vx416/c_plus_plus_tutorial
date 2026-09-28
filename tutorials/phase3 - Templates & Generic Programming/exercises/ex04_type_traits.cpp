/**
 * 練習 4: Type Traits Dispatch
 *
 * 實作 describe(T)，依照型別回傳不同描述：
 *   - integral: "integer"
 *   - floating point: "floating"
 *   - pointer: "pointer"
 *   - string: "string"
 *   - 其他: "unknown"
 *
 * 重點練習：type_traits、if constexpr、is_same_v
 */

#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>
using namespace std;

template <typename T>
string describe(const T&) {
    // TODO: 用 if constexpr 做型別分派
    // if constexpr 的分支在編譯期決定，只有符合的分支會被保留。
    // 順序有意義：pointer 也可能指向 integral，例如 int*，
    // 但 is_integral_v<int*> 是 false，所以這裡先判 pointer 更直覺。
    if constexpr (is_pointer_v<T>) {
        return "pointer";
    } else if constexpr (is_integral_v<T>) {
        // bool 也屬於 integral。若不想把 bool 算成 integer，要額外排除 is_same_v<T, bool>。
        return "integer";
    } else if constexpr (is_floating_point_v<T>) {
        return "floating";
    } else if constexpr (is_same_v<T, string>) {
        // 這裡只匹配 std::string。
        // 字串 literal "hello" 的型別不是 string，而是 const char[N]。
        return "string";
    } else {
        return "unknown";
    }
}

struct User {};

int main() {
    int x = 10;
    double y = 1.5;
    string s = "hello";
    User user;

    assert(describe(x) == "integer");
    assert(describe(y) == "floating");
    assert(describe(&x) == "pointer");
    assert(describe(s) == "string");
    assert(describe(user) == "unknown");

    cout << "ex04 passed!" << endl;
    return 0;
}
