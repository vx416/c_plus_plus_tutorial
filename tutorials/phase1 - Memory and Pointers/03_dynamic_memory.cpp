/**
 * Phase 1-3: Dynamic Memory
 *
 * Stack 上的變數在離開 scope 就自動釋放，但有時候我們需要：
 *   - 在 runtime 決定大小
 *   - 讓物件的生命週期超過建立它的 scope
 * 這時就需要 heap allocation：new / delete。
 *
 * 目錄:
 *   1. Stack vs Heap（配置位置與生命週期差異）
 *   2. new[] / delete[]（動態陣列）
 *   3. Memory Leak 常見模式（4 種情境）
 *   4. 動態物件（constructor / destructor 觸發時機）
 *   5. Leak 偵測工具（AddressSanitizer、Valgrind）
 */

#include <iostream>
#include <cstring>
using namespace std;

// ============================================================
// 1. Stack vs Heap
// ============================================================
void stack_vs_heap() {
    // Output:
    // === Stack vs Heap ===
    // stack_var: 42  (位址: <address>)
    // heap_var:  42  (位址: <address>)
    //
    cout << "=== Stack vs Heap ===" << endl;

    // Stack：自動管理，離開 scope 自動消失
    int stack_var = 42;

    // Heap：手動管理，需要自己 delete
    int* heap_var = new int(42);

    cout << "stack_var: " << stack_var << "  (位址: " << &stack_var << ")" << endl;
    cout << "heap_var:  " << *heap_var << "  (位址: " << heap_var << ")" << endl;

    delete heap_var;  // 必須手動釋放
    // heap_var 現在是 dangling pointer，不要再用它
    heap_var = nullptr;  // 好習慣：delete 後設為 nullptr
    cout << endl;
}

// ============================================================
// 2. new[] / delete[] — 動態陣列
// ============================================================
void dynamic_array() {
    // Output:
    // === 動態陣列 ===
    // 輸入陣列大小 (建議 5): 5
    // 陣列內容: 10 20 30 40 50 
    //
    cout << "=== 動態陣列 ===" << endl;

    int size;
    cout << "輸入陣列大小 (建議 5): ";
    size = 5;  // 為了自動執行，直接給值
    cout << size << endl;

    int* arr = new int[size];

    // 初始化
    for (int i = 0; i < size; ++i) {
        arr[i] = (i + 1) * 10;
    }

    // 使用
    cout << "陣列內容: ";
    for (int i = 0; i < size; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;

    // 注意：配對使用 new[] 和 delete[]
    delete[] arr;  // 不是 delete arr!
    arr = nullptr;
    cout << endl;
}

// ============================================================
// 3. Memory Leak 常見模式
// ============================================================
void memory_leak_examples() {
    // Output:
    // === Memory Leak 常見模式 ===
    // 模式 1: 忘記 delete
    //
    //     void leak() {
    //         int* p = new int(42);
    //         // ... 用完忘記 delete
    //         // p 離開 scope，位址遺失，記憶體永遠無法回收
    //     }
    //     
    // 模式 2: 提前 return
    //
    //     void leak(bool flag) {
    //         int* p = new int(42);
    //         if (flag) return;  // delete 永遠不會被執行！
    //         delete p;
    //     }
    //     
    // 模式 3: exception 中斷
    //
    //     void leak() {
    //         int* p = new int(42);
    //         do_something();    // 如果這裡 throw，delete 不會執行
    //         delete p;
    //     }
    //     
    // 模式 4: 覆蓋指標遺失舊位址
    //
    //     int* p = new int(1);
    //     p = new int(2);  // 第一塊記憶體的位址遺失了，永遠無法 delete
    //     delete p;        // 只釋放了第二塊
    //     
    // 解法：使用 RAII 和 Smart Pointers（見 1-4, 1-5）
    //
    cout << "=== Memory Leak 常見模式 ===" << endl;

    // --- 模式 1: 忘記 delete ---
    cout << "模式 1: 忘記 delete" << endl;
    cout << R"(
    void leak() {
        int* p = new int(42);
        // ... 用完忘記 delete
        // p 離開 scope，位址遺失，記憶體永遠無法回收
    }
    )" << endl;

    // --- 模式 2: 提前 return 跳過 delete ---
    cout << "模式 2: 提前 return" << endl;
    cout << R"(
    void leak(bool flag) {
        int* p = new int(42);
        if (flag) return;  // delete 永遠不會被執行！
        delete p;
    }
    )" << endl;

    // --- 模式 3: exception 跳過 delete ---
    cout << "模式 3: exception 中斷" << endl;
    cout << R"(
    void leak() {
        int* p = new int(42);
        do_something();    // 如果這裡 throw，delete 不會執行
        delete p;
    }
    )" << endl;

    // --- 模式 4: 覆蓋指標 ---
    cout << "模式 4: 覆蓋指標遺失舊位址" << endl;
    cout << R"(
    int* p = new int(1);
    p = new int(2);  // 第一塊記憶體的位址遺失了，永遠無法 delete
    delete p;        // 只釋放了第二塊
    )" << endl;

    cout << "解法：使用 RAII 和 Smart Pointers（見 1-4, 1-5）" << endl;
    cout << endl;
}

// ============================================================
// 4. 動態物件
// ============================================================
struct Student {
    string name;
    int score;

    Student(const string& n, int s) : name(n), score(s) {
        cout << "  [Student 建構] " << name << endl;
    }

    ~Student() {
        cout << "  [Student 解構] " << name << endl;
    }
};

void dynamic_object() {
    // Output:
    // === 動態物件 ===
    //   [Student 建構] Alice
    //   Alice 的分數: 95
    //   [Student 解構] Alice
    //
    // 動態物件陣列:
    //   [Student 建構] Bob
    //   [Student 建構] Charlie
    //   [Student 解構] Charlie
    //   [Student 解構] Bob
    //
    cout << "=== 動態物件 ===" << endl;

    Student* s = new Student("Alice", 95);
    cout << "  " << s->name << " 的分數: " << s->score << endl;
    delete s;  // 會呼叫 destructor

    cout << endl;

    // 動態物件陣列
    cout << "動態物件陣列:" << endl;
    Student* students = new Student[2]{
        {"Bob", 80},
        {"Charlie", 90}
    };
    delete[] students;  // 每個元素的 destructor 都會被呼叫
    cout << endl;
}

// ============================================================
// 5. 實用技巧：用 valgrind 或 sanitizer 偵測 leak
// ============================================================
void leak_detection_tips() {
    // Output:
    // === Leak 偵測工具 ===
    // 編譯時加上 AddressSanitizer:
    //   g++ -fsanitize=address -g example.cpp -o example
    //   ./example
    //
    // 或使用 Valgrind (Linux):
    //   valgrind --leak-check=full ./example
    //
    // 最佳解法：盡量用 smart pointer 取代 raw new/delete（見 1-5）
    cout << "=== Leak 偵測工具 ===" << endl;
    cout << "編譯時加上 AddressSanitizer:" << endl;
    cout << "  g++ -fsanitize=address -g example.cpp -o example" << endl;
    cout << "  ./example" << endl;
    cout << endl;
    cout << "或使用 Valgrind (Linux):" << endl;
    cout << "  valgrind --leak-check=full ./example" << endl;
    cout << endl;
    cout << "最佳解法：盡量用 smart pointer 取代 raw new/delete（見 1-5）" << endl;
    cout << endl;
}

int main() {
    stack_vs_heap();
    dynamic_array();
    memory_leak_examples();
    dynamic_object();
    leak_detection_tips();

    return 0;
}
