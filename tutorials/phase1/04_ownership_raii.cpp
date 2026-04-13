/**
 * Phase 1-4: Ownership & RAII
 *
 * RAII = Resource Acquisition Is Initialization
 * 核心思想：把資源的生命週期綁定到物件的生命週期。
 *   - 建構時取得資源（allocate / open / lock）
 *   - 解構時釋放資源（free / close / unlock）
 *
 * 這是 C++ 最重要的慣用法，也是 smart pointer 的基礎。
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
using namespace std;

// ============================================================
// 1. 沒有 RAII 的世界 — 手動管理資源
// ============================================================
void without_raii() {
    cout << "=== 沒有 RAII ===" << endl;

    // 手動 open + close，容易忘記或被 exception 跳過
    cout << R"(
    FILE* f = fopen("data.txt", "r");
    if (!f) return;

    char buf[256];
    fgets(buf, 256, f);
    // 如果中間 throw exception，fclose 永遠不會執行
    process(buf);  // 可能 throw!

    fclose(f);  // 容易忘記，或被跳過
    )" << endl;
}

// ============================================================
// 2. RAII 的解法 — 用物件包裝資源
// ============================================================

// 自己寫一個簡單的 RAII wrapper
class IntBuffer {
public:
    // 建構時取得資源
    explicit IntBuffer(size_t size)
        : data_(new int[size]), size_(size) {
        memset(data_, 0, size * sizeof(int));
        cout << "  [IntBuffer] 分配了 " << size_ << " 個 int" << endl;
    }

    // 解構時自動釋放 — 不管是正常離開還是 exception
    ~IntBuffer() {
        delete[] data_;
        cout << "  [IntBuffer] 已釋放記憶體" << endl;
    }

    // 禁止複製（避免 double free）
    IntBuffer(const IntBuffer&) = delete;
    IntBuffer& operator=(const IntBuffer&) = delete;

    int& operator[](size_t i) { return data_[i]; }
    size_t size() const { return size_; }

private:
    int* data_;
    size_t size_;
};

void with_raii() {
    cout << "=== 使用 RAII ===" << endl;

    {
        IntBuffer buf(5);
        buf[0] = 42;
        buf[1] = 100;

        cout << "  buf[0] = " << buf[0] << endl;
        cout << "  buf[1] = " << buf[1] << endl;
        // 離開這個 scope 時，IntBuffer 的 destructor 自動被呼叫
    }
    cout << "  scope 結束，記憶體已自動釋放" << endl;
    cout << endl;
}

// ============================================================
// 3. RAII + Exception Safety
// ============================================================
void raii_exception_safety() {
    cout << "=== RAII + Exception Safety ===" << endl;

    try {
        IntBuffer buf(3);
        buf[0] = 1;

        cout << "  準備 throw exception..." << endl;
        throw runtime_error("something went wrong");

        // 這行不會執行，但 buf 的 destructor 仍然會被呼叫！
    } catch (const exception& e) {
        cout << "  捕獲: " << e.what() << endl;
        cout << "  即使 exception 發生，IntBuffer 仍然正確釋放了記憶體" << endl;
    }
    cout << endl;
}

// ============================================================
// 4. RAII 不只管記憶體 — File 範例
// ============================================================
class FileGuard {
public:
    explicit FileGuard(const string& filename, ios_base::openmode mode)
        : file_(filename, mode) {
        if (!file_.is_open()) {
            throw runtime_error("無法開啟: " + filename);
        }
        cout << "  [FileGuard] 開啟 " << filename << endl;
    }

    ~FileGuard() {
        if (file_.is_open()) {
            file_.close();
            cout << "  [FileGuard] 已關閉檔案" << endl;
        }
    }

    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;

    fstream& stream() { return file_; }

private:
    fstream file_;
};

void raii_file_example() {
    cout << "=== RAII 管理檔案 ===" << endl;

    // 其實 std::fstream 本身就是 RAII — destructor 會自動 close
    // 這裡示範概念，實務上直接用 fstream 即可
    try {
        FileGuard guard("/tmp/raii_test.txt", ios::out);
        guard.stream() << "Hello RAII!" << endl;
        cout << "  寫入完成" << endl;
        // 離開 scope 自動關閉
    } catch (const exception& e) {
        cout << "  " << e.what() << endl;
    }
    cout << endl;
}

// ============================================================
// 5. Ownership 觀念
// ============================================================
void ownership_concept() {
    cout << "=== Ownership 觀念 ===" << endl;
    cout << "每個資源都應該有一個明確的 owner：" << endl;
    cout << "  - Owner 負責釋放資源" << endl;
    cout << "  - 其他人只是「借用」(borrow)" << endl;
    cout << "  - Owner 的生命週期結束 = 資源被釋放" << endl;
    cout << endl;
    cout << "C++ 的實踐方式：" << endl;
    cout << "  - unique_ptr → 獨佔 ownership（見 1-5）" << endl;
    cout << "  - shared_ptr → 共享 ownership（見 1-5）" << endl;
    cout << "  - raw pointer / reference → 只是借用，不負責釋放" << endl;
    cout << "  - 自訂 RAII class → 包裝任何資源（file, socket, lock...）" << endl;
    cout << endl;
}

int main() {
    without_raii();
    with_raii();
    raii_exception_safety();
    raii_file_example();
    ownership_concept();

    return 0;
}
