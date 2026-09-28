/**
 * Phase 8-4: CRTP
 *
 * CRTP: Curiously Recurring Template Pattern。
 * Derived 繼承 Base<Derived>，讓 base 在編譯期知道 derived 型別。
 *
 * 目錄:
 *   1. CRTP shape
 *   2. static polymorphism
 *   3. mixin
 *   4. compared with virtual
 *   5. 使用準則
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
// 1. CRTP shape
// ============================================================
// 本章重點：
//   CRTP 形狀是：
//     template <typename Derived> class Base {};
//     class Child : public Base<Child> {};
//
//   Base 可以用 static_cast<Derived&>(*this) 呼叫 Derived 的實作。
template <typename Derived>
class Printable {
public:
    void print() const {
        const Derived& self = static_cast<const Derived&>(*this);
        cout << "  " << self.to_string() << endl;
    }
};

class User : public Printable<User> {
public:
    explicit User(string name) : name_(std::move(name)) {}
    string to_string() const { return "User(" + name_ + ")"; }

private:
    string name_;
};

void shape_demo() {
    // Output:
    // === CRTP shape ===
    //   User(Alice)
    //
    cout << "=== CRTP shape ===" << endl;
    User user("Alice");
    user.print();
    cout << endl;
}

// ============================================================
// 2. static polymorphism
// ============================================================
// 本章重點：
//   virtual 是 runtime polymorphism。
//   CRTP 是 compile-time polymorphism，沒有 virtual dispatch，但型別要在編譯期知道。
template <typename Shape>
double area_of(const Shape& shape) {
    return shape.area();
}

class Square {
public:
    explicit Square(double side) : side_(side) {}
    double area() const { return side_ * side_; }

private:
    double side_;
};

void static_polymorphism_demo() {
    // Output:
    // === static polymorphism ===
    //   square area = 9
    //
    cout << "=== static polymorphism ===" << endl;
    cout << "  square area = " << area_of(Square(3.0)) << endl << endl;
}

// ============================================================
// 3. mixin
// ============================================================
// 本章重點：
//   mixin 是把一段可重用能力混進 class。
//   CRTP 常用來寫「需要 Derived 資訊」的 mixin。
template <typename Derived>
class EqualityById {
public:
    friend bool operator==(const Derived& lhs, const Derived& rhs) {
        return lhs.id() == rhs.id();
    }
};

class Order : public EqualityById<Order> {
public:
    explicit Order(int id) : id_(id) {}
    int id() const { return id_; }

private:
    int id_;
};

void mixin_demo() {
    // Output:
    // === mixin ===
    //   Order(1) == Order(1): true
    //
    cout << "=== mixin ===" << endl;
    cout << "  Order(1) == Order(1): " << boolalpha << (Order(1) == Order(1))
         << noboolalpha << endl << endl;
}

// ============================================================
// 4. compared with virtual
// ============================================================
// 本章重點：
//   virtual 適合 runtime 才知道具體型別，例如 vector<unique_ptr<Base>>。
//   CRTP 適合編譯期知道型別，且想避免 virtual 或提供 mixin。
void comparison_demo() {
    // Output:
    // === CRTP vs virtual ===
    //   virtual: runtime polymorphism
    //   CRTP: compile-time polymorphism
    //
    cout << "=== CRTP vs virtual ===" << endl;
    cout << "  virtual: runtime polymorphism" << endl;
    cout << "  CRTP: compile-time polymorphism" << endl << endl;
}

// ============================================================
// 5. 使用準則
// ============================================================
// 本章重點：
//   CRTP 很強，但錯誤訊息可能比 virtual 複雜。
//   需要 runtime plugin-like 擴充時，不要用 CRTP 取代 virtual。
void guideline_demo() {
    // Output:
    // === guideline ===
    //   use CRTP for mixins and static polymorphism
    //   use virtual when runtime substitution matters
    cout << "=== guideline ===" << endl;
    cout << "  use CRTP for mixins and static polymorphism" << endl;
    cout << "  use virtual when runtime substitution matters" << endl << endl;
}

int main() {
    shape_demo();
    static_polymorphism_demo();
    mixin_demo();
    comparison_demo();
    guideline_demo();
    return 0;
}
