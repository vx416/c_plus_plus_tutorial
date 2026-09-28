/**
 * 練習 4: Complex Number (Operator Overloading)
 *
 * 實作複數 Complex class，支援：
 *   - Constructor(real, imag)
 *   - operator+ / operator- / operator*  (複數運算)
 *   - operator== / operator!=
 *   - operator<<  (輸出格式 "a+bi" 或 "a-bi")
 *
 * 複數運算公式：
 *   (a + bi) + (c + di) = (a+c) + (b+d)i
 *   (a + bi) * (c + di) = (ac-bd) + (ad+bc)i
 *
 * 重點練習：成員 vs 非成員運算子、operator<<
 */

#include <sstream>
#include <cassert>
#include <iostream>
using namespace std;

class Complex {
public:
    // TODO: Constructor，儲存 real 和 imag
    Complex(double real, double imag) : real_(real), imag_(imag) {}

    // TODO: getter: real(), imag()（const）
    double real() const { return real_; }
    double imag() const { return imag_; }

    // TODO: operator+, operator-, operator*
    Complex operator+(const Complex& other) const {
        return Complex(real_ + other.real_, imag_ + other.imag_);
    }

    Complex operator-(const Complex& other) const {
        return Complex(real_ - other.real_, imag_ - other.imag_);
    }

    Complex operator*(const Complex& other) const {
        return Complex(real_ * other.real_ - imag_ * other.imag_,
                       real_ * other.imag_ + imag_ * other.real_);
    }

    // TODO: operator==, operator!=
    bool operator==(const Complex& other) const {
        return real_ == other.real_ && imag_ == other.imag_;
    }

    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

private:
    // TODO: 成員變數
    double real_;
    double imag_;
};

// TODO: 非成員 operator<<
// 格式：
//   real=3, imag=4 → "3+4i"
//   real=3, imag=-4 → "3-4i"
//   real=0, imag=5 → "0+5i"
// 提示：用 if 判斷 imag 正負，自己組合字串
ostream& operator<<(ostream& os, const Complex& c) {
    os << c.real();
    if (c.imag() >= 0) {
        os << '+';
    }
    return os << c.imag() << 'i';
}

string to_string(const Complex& c) {
    ostringstream oss;
    oss << c;
    return oss.str();
}

int main() {
    Complex a(3, 4);
    Complex b(1, 2);

    // 運算
    assert(to_string(a + b) == "4+6i");
    assert(to_string(a - b) == "2+2i");
    assert(to_string(a * b) == "-5+10i");  // (3+4i)(1+2i) = 3+6i+4i+8i² = -5+10i

    // 負虛部
    Complex c(3, -4);
    assert(to_string(c) == "3-4i");

    // 比較
    assert(a == Complex(3, 4));
    assert(a != b);

    cout << "ex04 passed!" << endl;
    return 0;
}
