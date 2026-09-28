/**
 * Phase 2-6: RTTI & dynamic_cast
 *
 * RTTI = Run-Time Type Information
 * 讓程式在執行時查詢物件的實際型別。
 *
 * C++ 的四種 cast:
 *   - static_cast     編譯時轉換，安全的「相容型別」
 *   - dynamic_cast    執行時檢查，處理多型階層的向下轉型
 *   - const_cast      加/去 const（少用）
 *   - reinterpret_cast 位元層級的重新解釋（危險）
 *
 * 目錄:
 *   1. typeid 與 type_info（查詢型別資訊）
 *   2. static_cast（編譯時轉換）
 *   3. dynamic_cast（執行時安全的向下轉型）
 *   4. const_cast / reinterpret_cast（少用）
 *   5. 什麼時候不該用 dynamic_cast（設計味道）
 *   6. C-style cast 為什麼要避免
 */

#include <iostream>
#include <typeinfo>
using namespace std;

// ============================================================
// 1. typeid 與 type_info
// ============================================================
/**
 * typeid(expr) 回傳 type_info，可查詢型別名稱、比較型別。
 * 對多型型別，typeid 會回傳「實際型別」而非 static 型別。
 */
class Base {
public:
    virtual ~Base() = default;
};

class Derived : public Base {};

void typeid_demo() {
    // Output:
    // === typeid ===
    //   typeid(int): i
    //   typeid(*p) = 7Derived  (實際型別是 Derived，不是 Base)
    //   *p 確實是 Derived 型別
    //
    cout << "=== typeid ===" << endl;

    int x = 42;
    cout << "  typeid(int): " << typeid(x).name() << endl;

    Derived d;
    Base* p = &d;

    cout << "  typeid(*p) = " << typeid(*p).name()
         << "  (實際型別是 Derived，不是 Base)" << endl;

    if (typeid(*p) == typeid(Derived)) {
        cout << "  *p 確實是 Derived 型別" << endl;
    }

    cout << endl;
}

// ============================================================
// 2. static_cast
// ============================================================
/**
 * 用於編譯時已知安全的轉換:
 *   - 數值轉換 (int ↔ double)
 *   - 向上轉型 (Derived* → Base*)
 *   - void* ↔ T*
 *   - enum ↔ int
 *
 * 不做執行時檢查，如果轉錯會是 undefined behavior。
 */
void static_cast_demo() {
    // Output:
    // === static_cast ===
    //   double→int: 3.7 → 3
    //   Derived* → Base*: OK (向上轉型永遠安全)
    //   Base* → Derived*: 編譯通過，但執行時不檢查（危險）
    //
    cout << "=== static_cast ===" << endl;

    // 數值轉換
    double d = 3.7;
    int i = static_cast<int>(d);
    cout << "  double→int: " << d << " → " << i << endl;

    // 向上轉型
    Derived dd;
    Base* bp = static_cast<Base*>(&dd);
    (void)bp;
    cout << "  Derived* → Base*: OK (向上轉型永遠安全)" << endl;

    // 向下轉型：編譯通過，但執行時不檢查 → 危險!
    Base base;
    Derived* dp = static_cast<Derived*>(&base);  // 編譯通過!
    (void)dp;
    cout << "  Base* → Derived*: 編譯通過，但執行時不檢查（危險）" << endl;

    cout << endl;
}

// ============================================================
// 3. dynamic_cast
// ============================================================
/**
 * 處理多型階層的向下轉型：
 *   - 轉成功：回傳正確的指標
 *   - 轉失敗：回傳 nullptr（對 pointer）或 throw bad_cast（對 reference）
 *
 * 要求：base class 必須有 virtual function（通常是 virtual destructor）。
 */
class Animal {
public:
    virtual ~Animal() = default;
    virtual string name() const = 0;
};

class Dog : public Animal {
public:
    string name() const override { return "Dog"; }
    void bark() const { cout << "  Woof!" << endl; }
};

class Cat : public Animal {
public:
    string name() const override { return "Cat"; }
    void meow() const { cout << "  Meow!" << endl; }
};

void handle(Animal* a) {
    cout << "  處理 " << a->name() << ":" << endl;

    // 嘗試轉成 Dog*
    if (Dog* d = dynamic_cast<Dog*>(a)) {
        d->bark();
    }
    // 嘗試轉成 Cat*
    else if (Cat* c = dynamic_cast<Cat*>(a)) {
        c->meow();
    }
    else {
        cout << "  未知動物" << endl;
    }
}

void dynamic_cast_demo() {
    // Output:
    // === dynamic_cast ===
    //   處理 Dog:
    //   Woof!
    //   處理 Cat:
    //   Meow!
    //
    cout << "=== dynamic_cast ===" << endl;

    Dog dog;
    Cat cat;

    handle(&dog);
    handle(&cat);

    cout << endl;
}

// ============================================================
// 4. const_cast 與 reinterpret_cast
// ============================================================
void other_casts_demo() {
    // Output:
    // === const_cast / reinterpret_cast ===
    //   const_cast: 去掉 const（修改原本 const 是 UB）
    //   reinterpret_cast: int 的 bytes = DCBA
    cout << "=== const_cast / reinterpret_cast ===" << endl;

    // const_cast: 加/去 const
    // 主要用於和不支援 const 的舊 C API 互動
    const int x = 10;
    const int* cp = &x;
    int* p = const_cast<int*>(cp);
    // *p = 20;  // undefined behavior! 不要修改原本就是 const 的變數
    (void)p;
    cout << "  const_cast: 去掉 const（修改原本 const 是 UB）" << endl;

    // reinterpret_cast: 位元層級重新解釋
    // 最危險的 cast，通常只在和硬體、序列化互動時用
    int n = 0x41424344;
    char* bytes = reinterpret_cast<char*>(&n);
    cout << "  reinterpret_cast: int 的 bytes = ";
    for (size_t i = 0; i < sizeof(int); ++i) {
        cout << bytes[i];
    }
    cout << endl;

    cout << endl;
}

// ============================================================
// 5. 什麼時候不該用 dynamic_cast
// ============================================================
/**
 * 如果你發現程式裡有大量 dynamic_cast，通常代表設計有問題。
 *
 * 壞味道的例子:
 *
 *   void process(Shape* s) {
 *       if (auto c = dynamic_cast<Circle*>(s)) {
 *           // 處理 Circle
 *       } else if (auto r = dynamic_cast<Rectangle*>(s)) {
 *           // 處理 Rectangle
 *       } // ...
 *   }
 *
 * 這應該改用 virtual function：
 *
 *   class Shape {
 *   public:
 *       virtual void process() = 0;   // 讓每個子類自己知道怎麼處理
 *   };
 *
 * dynamic_cast 合理的場景:
 *   - 真的無法透過 virtual 解決（少見）
 *   - 外部傳進來的 Base*，需要檢查是否是特定 derived
 *   - Visitor pattern 的某些實作
 */

// ============================================================
// 6. C-style cast 為什麼要避免
// ============================================================
/**
 * C-style 的 (T)x 其實會依序嘗試:
 *   1. const_cast
 *   2. static_cast
 *   3. static_cast + const_cast
 *   4. reinterpret_cast
 *   5. reinterpret_cast + const_cast
 *
 * 問題:
 *   - 看不出意圖（到底想做哪種轉換？）
 *   - 可能悄悄用到 reinterpret_cast → 隱藏危險
 *
 * 所以 C++ 分成四種 cast，強迫你明確表達意圖。
 *
 * 原則:
 *   數值/向上/已知安全  → static_cast
 *   多型向下           → dynamic_cast
 *   加/去 const        → const_cast
 *   位元重新解釋       → reinterpret_cast (極少用)
 *   C-style (T)x       → 不要用
 */

int main() {
    typeid_demo();
    static_cast_demo();
    dynamic_cast_demo();
    other_casts_demo();
    return 0;
}
