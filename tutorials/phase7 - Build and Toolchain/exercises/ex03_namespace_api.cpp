/**
 * 練習 3: Namespace API
 *
 * 把 public API 放進 app::math namespace。
 *
 * 重點練習：
 *   - 避免全域名字污染
 *   - 呼叫端用明確 namespace 表達來源
 */

#include <cassert>
#include <iostream>
using namespace std;

namespace app::math {
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}
}

int main() {
    assert(app::math::add(2, 3) == 5);
    assert(app::math::multiply(4, 5) == 20);

    cout << "ex03 passed!" << endl;
    return 0;
}
