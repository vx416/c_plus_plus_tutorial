/**
 * 練習 4: Static Library Shape
 *
 * 用單檔模擬 library API 和 implementation 邊界。
 *
 * 重點練習：
 *   - header 提供 declaration
 *   - source/library 提供 definition
 *   - 呼叫端只依賴 public API
 */

#include <cassert>
#include <iostream>
using namespace std;

// 假裝這段在 math.hpp
namespace mathlib {
int add(int a, int b);
int subtract(int a, int b);
}

// 假裝這段在 math.cpp，會被編成 math.o / libmath.a
namespace mathlib {
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}
}

int main() {
    assert(mathlib::add(1, 2) == 3);
    assert(mathlib::subtract(5, 3) == 2);

    cout << "ex04 passed!" << endl;
    return 0;
}
