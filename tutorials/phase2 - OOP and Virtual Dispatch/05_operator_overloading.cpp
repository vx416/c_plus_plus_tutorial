/**
 * Phase 2-5: Operator Overloading
 *
 * C++ 允許自訂型別對運算子的行為，讓自訂型別像 built-in 型別一樣使用。
 *
 * 目錄:
 *   1. 算術運算子（+ - * /）
 *   2. 比較運算子（== != < <=）
 *   3. 串流運算子（<< 搭配 std::ostream）
 *   4. 下標運算子（[]）
 *   5. 遞增 / 遞減（++ 前置 vs 後置）
 *   6. 成員 vs 非成員（friend）
 *   7. 不該 overload 的運算子
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. 算術運算子：以 2D 向量為例
// ============================================================
struct Vec2 {
    double x, y;

    // 成員函式形式：左運算元是自己（this）
    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2& other) const {
        return {x - other.x, y - other.y};
    }

    // 純量乘法
    Vec2 operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    // 複合賦值（通常回傳 *this 以支援連鎖）
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

// 非成員函式形式：讓 5.0 * v 也能用（左運算元不是 Vec2）
Vec2 operator*(double scalar, const Vec2& v) {
    return v * scalar;
}

ostream& operator<<(ostream& os, const Vec2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

void arithmetic_demo() {
    // Output:
    // === 算術運算子 ===
    //   a + b = (4, 6)
    //   a - b = (-2, -2)
    //   a * 2 = (2, 4)
    //   3 * b = (9, 12)
    //   a += b 後: (4, 6)
    //
    cout << "=== 算術運算子 ===" << endl;

    Vec2 a{1, 2};
    Vec2 b{3, 4};

    cout << "  a + b = " << (a + b) << endl;
    cout << "  a - b = " << (a - b) << endl;
    cout << "  a * 2 = " << (a * 2) << endl;
    cout << "  3 * b = " << (3.0 * b) << endl;  // 需要非成員版本

    a += b;
    cout << "  a += b 後: " << a << endl;

    cout << endl;
}

// ============================================================
// 2. 比較運算子
// ============================================================
/**
 * C++20 之前：要自己實作 == != < <= > >= 六個
 * C++20 起：實作 <=> (spaceship) 就自動產生其他五個
 */
class Version {
public:
    Version(int maj, int min) : major_(maj), minor_(min) {}

    bool operator==(const Version& other) const {
        return major_ == other.major_ && minor_ == other.minor_;
    }

    bool operator!=(const Version& other) const {
        return !(*this == other);
    }

    bool operator<(const Version& other) const {
        if (major_ != other.major_) return major_ < other.major_;
        return minor_ < other.minor_;
    }

    friend ostream& operator<<(ostream& os, const Version& v) {
        return os << v.major_ << "." << v.minor_;
    }

private:
    int major_, minor_;
};

void comparison_demo() {
    // Output:
    // === 比較運算子 ===
    //   v1 = 1.5, v2 = 2.0
    //   v1 == v2: 0
    //   v1 <  v2: 1
    //   v1 != v2: 1
    //
    cout << "=== 比較運算子 ===" << endl;

    Version v1(1, 5);
    Version v2(2, 0);

    cout << "  v1 = " << v1 << ", v2 = " << v2 << endl;
    cout << "  v1 == v2: " << (v1 == v2) << endl;
    cout << "  v1 <  v2: " << (v1 < v2) << endl;
    cout << "  v1 != v2: " << (v1 != v2) << endl;

    cout << endl;
}

// ============================================================
// 3. 下標運算子 []
// ============================================================
class IntArray {
public:
    explicit IntArray(size_t size) : size_(size), data_(new int[size]()) {}
    ~IntArray() { delete[] data_; }

    IntArray(const IntArray&) = delete;
    IntArray& operator=(const IntArray&) = delete;

    // 兩個版本：non-const 可寫，const 只能讀
    int& operator[](size_t i) { return data_[i]; }
    const int& operator[](size_t i) const { return data_[i]; }

    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

void subscript_demo() {
    // Output:
    // === 下標運算子 [] ===
    //   ref[0] = 10
    //
    cout << "=== 下標運算子 [] ===" << endl;

    IntArray arr(3);
    arr[0] = 10;    // 呼叫 non-const 版本 (可寫)
    arr[1] = 20;
    arr[2] = 30;

    const IntArray& ref = arr;
    cout << "  ref[0] = " << ref[0] << endl;  // 呼叫 const 版本
    // ref[0] = 99;  // 編譯錯誤！const 版本回傳 const&

    cout << endl;
}

// ============================================================
// 4. 遞增 / 遞減（前置 vs 後置）
// ============================================================
/**
 * 前置 ++x: 改值 → 回傳新值的 reference
 * 後置 x++: 先存舊值 → 改值 → 回傳舊值（by value）
 *
 * 後置版本有個「假的 int 參數」讓編譯器分辨兩者。
 */
class Counter {
public:
    Counter(int n = 0) : n_(n) {}

    // 前置 ++x
    Counter& operator++() {
        ++n_;
        return *this;
    }

    // 後置 x++（多一個 int 參數）
    Counter operator++(int) {
        Counter old = *this;  // 存舊值
        ++n_;
        return old;           // 回傳舊值
    }

    int value() const { return n_; }

private:
    int n_;
};

void increment_demo() {
    // Output:
    // === 遞增運算子 ===
    //   ++c: 6  (前置: 先改再回傳)
    //   c++: 6  (後置: 先回傳再改)
    //   c:   7
    //   實務建議：優先用前置 (++c)，效能較好，不會產生副本
    //
    cout << "=== 遞增運算子 ===" << endl;

    Counter c(5);
    cout << "  ++c: " << (++c).value() << "  (前置: 先改再回傳)" << endl;
    cout << "  c++: " << (c++).value() << "  (後置: 先回傳再改)" << endl;
    cout << "  c:   " << c.value() << endl;

    cout << "  實務建議：優先用前置 (++c)，效能較好，不會產生副本" << endl;
    cout << endl;
}

// ============================================================
// 5. 成員 vs 非成員（friend）
// ============================================================
/**
 * 成員函式:    a + b  →  a.operator+(b)
 * 非成員函式:  a + b  →  operator+(a, b)
 *
 * 什麼時候用非成員?
 *   - 當「左運算元不是你的 class」時（例如 5 * v 的 5 是 double）
 *   - 對稱運算子（通常放外面比較對稱）
 *
 * friend 關鍵字：讓非成員函式能存取 private 成員
 */
class Money {
public:
    Money(int dollars) : dollars_(dollars) {}

    // 讓非成員 operator+ 存取 private
    friend Money operator+(const Money& a, const Money& b);
    friend ostream& operator<<(ostream& os, const Money& m);

private:
    int dollars_;
};

Money operator+(const Money& a, const Money& b) {
    return Money(a.dollars_ + b.dollars_);  // 存取 private ✓
}

ostream& operator<<(ostream& os, const Money& m) {
    return os << "$" << m.dollars_;
}

void friend_demo() {
    // Output:
    // === friend 非成員運算子 ===
    //   a + b = $150
    cout << "=== friend 非成員運算子 ===" << endl;
    Money a(100);
    Money b(50);
    cout << "  a + b = " << (a + b) << endl;
    cout << endl;
}

// ============================================================
// 6. 不該 overload 的運算子
// ============================================================
/**
 * 技術上可以，但行為會很奇怪，請避免:
 *
 *   &&  ||    短路求值會失效（會變成一般函式呼叫，先算所有參數）
 *   ,         會破壞逗號序列的語意
 *   &         取址通常不該改
 *   ::        完全不能 overload
 *   ?:        完全不能 overload
 *   sizeof    完全不能 overload
 *
 * 最佳實踐：
 *   - 讓運算子的語意和 built-in 型別「一致」（例如 + 是加法，不是印訊息）
 *   - 對稱運算子用非成員
 *   - 一定要同時 overload 對稱的運算子（== 就要配 !=）
 */

int main() {
    arithmetic_demo();
    comparison_demo();
    subscript_demo();
    increment_demo();
    friend_demo();
    return 0;
}
