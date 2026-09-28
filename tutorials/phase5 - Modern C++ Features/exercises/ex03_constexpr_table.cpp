/**
 * 練習 3: constexpr Table
 *
 * 在編譯期建立 0..N-1 的平方表。
 *
 * 重點練習：
 *   - constexpr function
 *   - non-type template parameter
 *   - std::array
 */

#include <array>
#include <cassert>
#include <iostream>
using namespace std;

template <size_t N>
constexpr array<int, N> make_square_table() {
    // TODO: 這個 function 可以在編譯期建立 array。
    array<int, N> table{};
    for (size_t i = 0; i < N; ++i) {
        table[i] = static_cast<int>(i * i);
    }
    return table;
}

int main() {
    constexpr auto table = make_square_table<6>();
    static_assert(table[0] == 0);
    static_assert(table[3] == 9);
    static_assert(table[5] == 25);

    assert(table[4] == 16);

    cout << "ex03 passed!" << endl;
    return 0;
}
