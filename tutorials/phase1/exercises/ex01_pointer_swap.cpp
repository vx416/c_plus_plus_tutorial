/**
 * 練習 1: Pointer Swap
 *
 * 實作 my_swap，透過指標交換兩個整數的值。
 * 不能使用 reference，只能用 pointer。
 */

#include <iostream>
#include <cassert>
using namespace std;

void my_swap(int* a, int* b) {
    // TODO: 實作交換邏輯
}

int main() {
    int x = 10, y = 20;
    my_swap(&x, &y);
    assert(x == 20 && y == 10);

    int a = -1, b = 0;
    my_swap(&a, &b);
    assert(a == 0 && b == -1);

    cout << "ex01 passed!" << endl;
    return 0;
}
