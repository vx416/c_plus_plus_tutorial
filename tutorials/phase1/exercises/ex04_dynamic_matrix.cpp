/**
 * 練習 4: Dynamic Matrix
 *
 * 用 new/delete 實作一個動態 2D 矩陣。
 *   - create_matrix: 配置 rows x cols 的矩陣，初始值為 0
 *   - destroy_matrix: 釋放所有記憶體
 *   - set / get: 存取元素
 *
 * 重點：確保沒有 memory leak（每個 new 都要有對應的 delete）。
 */

#include <iostream>
#include <cassert>
using namespace std;

int** create_matrix(int rows, int cols) {
    // TODO: 配置 rows x cols 的 2D 矩陣，所有元素初始化為 0
    // 提示：先 new 一個 int* 陣列，再對每個 row new 一個 int 陣列
    return nullptr; // placeholder
}

void destroy_matrix(int** matrix, int rows) {
    // TODO: 釋放 create_matrix 配置的所有記憶體
}

void set(int** matrix, int row, int col, int value) {
    matrix[row][col] = value;
}

int get(int** matrix, int row, int col) {
    return matrix[row][col];
}

int main() {
    int rows = 3, cols = 4;
    int** m = create_matrix(rows, cols);
    assert(m != nullptr);

    // 初始值應為 0
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            assert(get(m, r, c) == 0);

    // 設值與取值
    set(m, 0, 0, 1);
    set(m, 1, 2, 42);
    set(m, 2, 3, -7);
    assert(get(m, 0, 0) == 1);
    assert(get(m, 1, 2) == 42);
    assert(get(m, 2, 3) == -7);

    destroy_matrix(m, rows);

    cout << "ex04 passed!" << endl;
    cout << "（建議用 -fsanitize=address 編譯確認沒有 memory leak）" << endl;
    return 0;
}
