/**
 * Phase 2-3: Inheritance
 *
 * 繼承讓子類別「繼承」父類別的成員，實現程式碼重用與型別階層。
 *
 * 目錄:
 *   1. 基本繼承（public / protected / private 三種繼承方式）
 *   2. Constructor / Destructor 的呼叫順序
 *   3. protected 成員（子類別可見）
 *   4. virtual destructor（透過 base pointer 刪除時必要）
 *   5. Slicing 問題（by-value 會切掉 derived 的部分）
 *   6. 組合 vs 繼承（is-a vs has-a）
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
// 1. 基本繼承
// ============================================================
class Animal {
public:
    Animal(const string& name) : name_(name) {
        cout << "  [Animal ctor] " << name_ << endl;
    }
    ~Animal() {
        cout << "  [Animal dtor] " << name_ << endl;
    }

    void eat() const { cout << "  " << name_ << " is eating" << endl; }

protected:
    string name_;  // 子類別可以存取
};

// 繼承語法: class Child : <access> Parent
// - public 繼承（最常見）：is-a 關係
// - protected 繼承：很少用
// - private 繼承：通常改用組合
class Dog : public Animal {
public:
    Dog(const string& name) : Animal(name) {
        cout << "  [Dog ctor] " << endl;
    }
    ~Dog() {
        cout << "  [Dog dtor]" << endl;
    }

    void bark() const {
        cout << "  " << name_ << " says: Woof!" << endl;  // 可存取 protected
    }
};

void basic_inheritance_demo() {
    // Output:
    // === 基本繼承 ===
    //   [Animal ctor] Rex
    //   [Dog ctor] 
    //   Rex is eating
    //   Rex says: Woof!
    //
    //   [Dog dtor]
    //   [Animal dtor] Rex
    cout << "=== 基本繼承 ===" << endl;

    Dog d("Rex");
    d.eat();    // 繼承自 Animal
    d.bark();   // Dog 自己的

    cout << endl;
}

// ============================================================
// 2. Constructor / Destructor 的呼叫順序
// ============================================================
/**
 * 建構順序：base → derived（由上到下）
 * 解構順序：derived → base（由下到上，與建構相反）
 *
 * 理由：derived 可能依賴 base 的成員，所以 base 必須先建構、最後解構。
 */
void constructor_order_demo() {
    // Output:
    // === Constructor / Destructor 順序 ===
    //   建立 Dog:
    //   [Animal ctor] Buddy
    //   [Dog ctor] 
    //   [Dog dtor]
    //   [Animal dtor] Buddy
    //
    cout << "=== Constructor / Destructor 順序 ===" << endl;
    cout << "  建立 Dog:" << endl;
    {
        Dog d("Buddy");
        // 觀察：先印 Animal ctor，再印 Dog ctor
    }
    // 離開 scope：先印 Dog dtor，再印 Animal dtor
    cout << endl;
}

// ============================================================
// 3. Virtual Destructor（非常重要）
// ============================================================
/**
 * 問題：透過 base pointer 刪除 derived 物件，如果 destructor 不是 virtual，
 *       只會呼叫 base 的 destructor，derived 的 destructor 不會被執行 → 資源洩漏。
 */
class BadBase {
public:
    ~BadBase() { cout << "  [BadBase dtor]" << endl; }  // 沒有 virtual!
};

class BadDerived : public BadBase {
public:
    BadDerived() : data_(new int[100]) {}
    ~BadDerived() {
        delete[] data_;
        cout << "  [BadDerived dtor] (釋放 data_)" << endl;
    }
private:
    int* data_;
};

class GoodBase {
public:
    virtual ~GoodBase() { cout << "  [GoodBase dtor]" << endl; }  // virtual ✓
};

class GoodDerived : public GoodBase {
public:
    GoodDerived() : data_(new int[100]) {}
    ~GoodDerived() override {   // 可以加 override 讓編譯器檢查
        delete[] data_;
        cout << "  [GoodDerived dtor] (釋放 data_)" << endl;
    }
private:
    int* data_;
};

void virtual_destructor_demo() {
    // Output:
    // === Virtual Destructor ===
    //   沒有 virtual dtor（BAD）:
    //   [BadBase dtor]
    //   有 virtual dtor（GOOD）:
    //   [GoodDerived dtor] (釋放 data_)
    //   [GoodBase dtor]
    //   規則：只要 class 打算當 base class，destructor 一律 virtual
    //
    cout << "=== Virtual Destructor ===" << endl;

    cout << "  沒有 virtual dtor（BAD）:" << endl;
    {
        BadBase* p = new BadDerived();
        delete p;  // 只呼叫 BadBase 的 dtor，BadDerived 的 data_ 洩漏!
    }

    cout << "  有 virtual dtor（GOOD）:" << endl;
    {
        GoodBase* p = new GoodDerived();
        delete p;  // 會呼叫 GoodDerived 和 GoodBase 的 dtor
    }

    cout << "  規則：只要 class 打算當 base class，destructor 一律 virtual" << endl;
    cout << endl;
}

// ============================================================
// 4. Slicing 問題
// ============================================================
/**
 * 如果你 by-value 傳 derived 物件給要 base 的地方，derived 的部分會被「切掉」。
 */
class Shape {
public:
    virtual ~Shape() = default;
    virtual string name() const { return "Shape"; }
};

class Circle : public Shape {
public:
    string name() const override { return "Circle"; }
};

void print_shape_by_value(Shape s) {    // 注意：by value
    cout << "  by value:  " << s.name() << endl;  // 永遠是 "Shape"
}

void print_shape_by_ref(const Shape& s) {
    cout << "  by ref:    " << s.name() << endl;  // 正確：Circle
}

void slicing_demo() {
    // Output:
    // === Slicing 問題 ===
    //   by value:  Shape
    //   by ref:    Circle
    //   規則：處理 polymorphic 物件一律用 reference 或 pointer
    //
    cout << "=== Slicing 問題 ===" << endl;

    Circle c;
    print_shape_by_value(c);  // Circle 的部分被切掉了!
    print_shape_by_ref(c);    // 正確保留 derived 型別

    cout << "  規則：處理 polymorphic 物件一律用 reference 或 pointer" << endl;
    cout << endl;
}

// ============================================================
// 5. 組合 vs 繼承
// ============================================================
/**
 * 組合 (has-a): 成員變數裡「有一個」別的物件
 * 繼承 (is-a):  「是一個」別的物件的特化版本
 *
 * 優先選組合，繼承只在真的有 is-a 關係時使用。
 * 原因：繼承會強綁定 base 的介面，修改 base 會影響所有 derived。
 */
class Engine {
public:
    void start() { cout << "  Engine started" << endl; }
};

// has-a: Car 有一個 Engine（組合）
class Car {
public:    void drive() {
        engine_.start();  // 用 engine 的功能，但 Car 不是 Engine
        cout << "  Car is moving" << endl;
    }
private:
    Engine engine_;
};

void composition_demo() {
    // Output:
    // === 組合 vs 繼承 ===
    //   Engine started
    //   Car is moving
    //   Car 有引擎 (has-a)，不是繼承 Engine
    cout << "=== 組合 vs 繼承 ===" << endl;
    Car c;
    c.drive();
    cout << "  Car 有引擎 (has-a)，不是繼承 Engine" << endl;
    cout << endl;
}

int main() {
    basic_inheritance_demo();
    constructor_order_demo();
    virtual_destructor_demo();
    slicing_demo();
    composition_demo();
    return 0;
}
