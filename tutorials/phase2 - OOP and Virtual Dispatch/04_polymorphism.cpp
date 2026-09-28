/**
 * Phase 2-4: Polymorphism
 *
 * Polymorphism = 同一個介面，不同的實作行為。
 * C++ 的 runtime polymorphism 透過 virtual function 實現。
 *
 * 目錄:
 *   1. virtual function（動態綁定）
 *   2. pure virtual 與 abstract class（介面）
 *   3. override 與 final 關鍵字
 *   4. vtable 機制（編譯器怎麼實現 virtual）
 *   5. 透過 base pointer 的多型使用
 *   6. 多型容器（vector<unique_ptr<Base>>）
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

// ============================================================
// 1. virtual function
// ============================================================
/**
 * 不加 virtual：函式呼叫在**編譯時**決定（靜態綁定）
 * 加 virtual：  函式呼叫在**執行時**決定（動態綁定，根據實際物件型別）
 */
class ShapeA {
public:
    void draw() const { cout << "  ShapeA::draw" << endl; }         // 非 virtual
    virtual void area() const { cout << "  ShapeA::area" << endl; } // virtual
    virtual ~ShapeA() = default;
};

class CircleA : public ShapeA {
public:
    void draw() const { cout << "  CircleA::draw" << endl; }
    void area() const override { cout << "  CircleA::area" << endl; }
};

void virtual_demo() {
    // Output:
    // === virtual function ===
    //   ShapeA::draw
    //   CircleA::area
    //   結論：想要多型行為，就加 virtual
    //
    cout << "=== virtual function ===" << endl;

    CircleA c;
    ShapeA* p = &c;

    p->draw();   // 靜態綁定：呼叫 ShapeA::draw（根據 pointer 型別）
    p->area();   // 動態綁定：呼叫 CircleA::area（根據實際物件型別）

    cout << "  結論：想要多型行為，就加 virtual" << endl;
    cout << endl;
}

// ============================================================
// 2. pure virtual 與 abstract class
// ============================================================
/**
 * = 0 宣告「pure virtual」：必須在子類別實作。
 * 含有 pure virtual 的 class 叫 abstract class，不能實例化。
 *
 * 等同於其他語言的「interface」。
 */
class Shape {
public:
    virtual ~Shape() = default;

    // 純虛擬：子類別必須實作
    virtual double area() const = 0;
    virtual string name() const = 0;

    // 一般 virtual：子類別可以覆寫（有預設實作）
    virtual void describe() const {
        cout << "  " << name() << " area = " << area() << endl;
    }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : radius_(r) {}

    double area() const override { return 3.14159 * radius_ * radius_; }
    string name() const override { return "Circle"; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}

    double area() const override { return width_ * height_; }
    string name() const override { return "Rectangle"; }

private:
    double width_, height_;
};

void abstract_class_demo() {
    // Output:
    // === Abstract Class ===
    //   Circle area = 12.5664
    //   Rectangle area = 12
    //
    cout << "=== Abstract Class ===" << endl;

    // Shape s;  // 編譯錯誤！abstract class 不能實例化
    Circle c(2);
    Rectangle r(3, 4);

    c.describe();
    r.describe();

    cout << endl;
}

// ============================================================
// 3. override 與 final
// ============================================================
/**
 * override: 明確表示「我在覆寫 base 的 virtual function」
 *           編譯器會檢查，簽名不對會報錯（防止打錯字）
 *
 * final: 禁止進一步覆寫或繼承
 */
class Base {
public:
    virtual void foo() const { cout << "  Base::foo" << endl; }
    virtual void bar() const { cout << "  Base::bar" << endl; }
    virtual ~Base() = default;
};

class Mid : public Base {
public:
    // 加 override：編譯器會檢查
    void foo() const override { cout << "  Mid::foo" << endl; }

    // final：禁止再被覆寫
    void bar() const override final { cout << "  Mid::bar (final)" << endl; }

    // 如果寫錯簽名（例如少了 const），沒有 override 會被當成新函式
    // void foo() { ... }              // 不會報錯，但不是 override!
    // void foo() override { ... }     // 加 override → 編譯錯誤，提醒你修正
};

class Last : public Mid {
public:
    void foo() const override { cout << "  Last::foo" << endl; }
    // void bar() const override {}  // 編譯錯誤！Mid::bar 是 final
};

// 整個 class 加 final：禁止被繼承
class NoInherit final {};
// class X : public NoInherit {};  // 編譯錯誤

void override_final_demo() {
    // Output:
    // === override / final ===
    //   Last::foo
    //   Mid::bar (final)
    //
    cout << "=== override / final ===" << endl;
    Last l;
    l.foo();
    l.bar();   // 呼叫 Mid::bar（Last 不能覆寫）
    cout << endl;
}

// ============================================================
// 4. vtable 機制（簡述）
// ============================================================
/**
 * 編譯器實現 virtual 的方式：
 *
 * 1. 每個有 virtual 的 class 有一張 vtable（函式指標表）
 *    vtable 內容:
 *      Shape::vtable:       Circle::vtable:
 *        [0] Shape::area      [0] Circle::area  ← 覆寫了
 *        [1] Shape::name      [1] Circle::name
 *
 * 2. 每個物件開頭有一個隱藏的 vptr，指向自己 class 的 vtable
 *    Circle 物件:
 *      +--------+
 *      | vptr   | ──→ Circle::vtable
 *      | radius |
 *      +--------+
 *
 * 3. 呼叫 p->area() 時：
 *    - 不是 virtual: 直接呼叫 Shape::area (編譯時決定)
 *    - 是 virtual:   查 p->vptr[area] → 得到 Circle::area (執行時決定)
 *
 * 代價：每個物件多一個 pointer (vptr)，virtual 呼叫多一次間接查表。
 */

// ============================================================
// 5. 透過 base pointer 的多型使用
// ============================================================
void print_shape(const Shape& s) {   // 只知道是 Shape
    s.describe();                    // 但會呼叫實際型別的 area / name
}

void polymorphic_usage_demo() {
    // Output:
    // === 多型使用 ===
    //   Circle area = 78.5397
    //   Rectangle area = 6
    //
    cout << "=== 多型使用 ===" << endl;

    Circle c(5);
    Rectangle r(2, 3);

    print_shape(c);  // 呼叫 Circle::area
    print_shape(r);  // 呼叫 Rectangle::area

    cout << endl;
}

// ============================================================
// 6. 多型容器
// ============================================================
/**
 * 想把不同子類別放在同一個容器？不能用 vector<Shape>（slicing + abstract）
 * 要用 vector<unique_ptr<Shape>> 或 vector<shared_ptr<Shape>>。
 */
void polymorphic_container_demo() {
    // Output:
    // === 多型容器 ===
    //   Circle area = 3.14159
    //   Rectangle area = 6
    //   Circle area = 78.5397
    //   總面積: 87.6813
    cout << "=== 多型容器 ===" << endl;

    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>(1));
    shapes.push_back(make_unique<Rectangle>(2, 3));
    shapes.push_back(make_unique<Circle>(5));

    double total = 0;
    for (const auto& s : shapes) {
        s->describe();
        total += s->area();
    }
    cout << "  總面積: " << total << endl;
    cout << endl;
}

int main() {
    virtual_demo();
    abstract_class_demo();
    override_final_demo();
    polymorphic_usage_demo();
    polymorphic_container_demo();
    return 0;
}
