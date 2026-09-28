/**
 * 練習 2: Array Reverse (Pointer Arithmetic)
 *
 * 用指標（不是 index）原地反轉一個 int 陣列。
 * 提示：用兩個指標，一個從頭、一個從尾，向中間靠攏。
 */

#include <iostream>
#include <cassert>
using namespace std;

void reverse_array(int* arr, int size) {
    // TODO: 只用指標操作，不要用 arr[i]
    int l = 0;
    int r = size - 1;

    while (l < r ) {
        int tmp = *(arr + l);
        *(arr + l) = *(arr + r);
        *(arr + r) = tmp;
        
        l++;
        r--;
    }
}

int main() {
    int a[] = {1, 2, 3, 4, 5};
    reverse_array(a, 5);
    assert(a[0] == 5 && a[1] == 4 && a[2] == 3 && a[3] == 2 && a[4] == 1);

    int b[] = {10, 20};
    reverse_array(b, 2);
    assert(b[0] == 20 && b[1] == 10);

    int c[] = {42};
    reverse_array(c, 1);
    assert(c[0] == 42);

    cout << "ex02 passed!" << endl;
    return 0;
}
