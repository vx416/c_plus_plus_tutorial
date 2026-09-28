/**
 * Phase 2-1: Class & Struct
 *
 * class 和 struct 在 C++ 裡幾乎一樣，差別只有：
 *   - struct 預設 public
 *   - class  預設 private
 *
 * 慣例：
 *   - struct 用於「純資料」（所有成員 public，沒什麼邏輯）
 *   - class  用於「有封裝」（資料 private，透過方法操作）
 *
 * 目錄:
 *   1. struct: 純資料容器
 *   2. class:  封裝與 access specifier
 *   3. Constructor 的各種形式（預設 / 參數 / explicit / delegating）
 *   4. 成員初始化列表 (member initializer list)
 *   5. Access Specifier 總整理（public / private / protected）
 *   6. const 成員函式 / mutable
 *   7. static 成員（class-level 變數與方法）
 *   8. 修飾詞組合速查表
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
// 1. struct：純資料容器
// ============================================================
struct Point {
    double x;
    double y;
    // 沒寫 access specifier，預設 public
};

void struct_demo() {
    // Output:
    // === struct ===
    //   p1: (3, 4)
    //   修改後: (10, 4)
    //
    cout << "=== struct ===" << endl;

    Point p1{3.0, 4.0};  // aggregate initialization
    cout << "  p1: (" << p1.x << ", " << p1.y << ")" << endl;

    p1.x = 10;  // 可以直接改
    cout << "  修改後: (" << p1.x << ", " << p1.y << ")" << endl;
    cout << endl;
}

// ============================================================
// 2. class：封裝與 access specifier
// ============================================================
class BankAccount {
public:  // 外部可以存取
    // 建構子
    BankAccount(const string& owner, double initial_balance)
        : owner_(owner), balance_(initial_balance) {
        cout << "  [建構] 帳戶開立: " << owner_ << endl;
    }

    // 解構子
    ~BankAccount() {
        cout << "  [解構] 帳戶關閉: " << owner_ << endl;
    }

    // 公開方法（interface）
    void deposit(double amount) {
        if (amount > 0) balance_ += amount;
    }

    bool withdraw(double amount) {
        if (amount > balance_) return false;
        balance_ -= amount;
        return true;
    }

    double balance() const { return balance_; }
    const string& owner() const { return owner_; }

private:  // 只有 class 內部能存取
    string owner_;
    double balance_;

    // private 方法（內部實作細節）
    void log_transaction(double amount) {
        cout << "  [log] " << owner_ << " transaction: " << amount << endl;
    }
};

void class_demo() {
    // Output:
    // === class ===
    //   [建構] 帳戶開立: Alice
    //   存款後餘額: 1500
    //   提款失敗（餘額不足）
    //
    //   [解構] 帳戶關閉: Alice
    cout << "=== class ===" << endl;

    BankAccount acc("Alice", 1000);
    acc.deposit(500);
    cout << "  存款後餘額: " << acc.balance() << endl;

    if (acc.withdraw(2000)) {
        cout << "  提款成功" << endl;
    } else {
        cout << "  提款失敗（餘額不足）" << endl;
    }

    // acc.balance_ = 999999;  // 編譯錯誤！private
    // acc.log_transaction(0); // 編譯錯誤！private 方法
    cout << endl;
}

// ============================================================
// 3. Constructor 的各種形式
// ============================================================
/**
 * --- explicit 是什麼？---
 *
 * 沒加 explicit 的「單一參數 constructor」會被編譯器當成「隱式轉換規則」：
 *
 *     class Distance {
 *     public:
 *         Distance(int meters) { ... }   // 沒加 explicit
 *     };
 *
 *     void walk(Distance d) { ... }
 *
 *     walk(100);  // 編譯通過！編譯器偷偷把 100 轉成 Distance(100)
 *                 // 但你本來可能想傳「100 步」而不是「100 公尺」
 *
 * 加了 explicit 就會禁止這種隱式轉換，強迫你明確寫出型別：
 *
 *     class Distance {
 *     public:
 *         explicit Distance(int meters) { ... }
 *     };
 *
 *     walk(100);            // 編譯錯誤！防止踩雷
 *     walk(Distance(100));  // OK，明確表達意圖
 *     walk(Distance{100});  // OK
 *
 *
 * --- explicit 的好處 ---
 *
 * 1. 防止意外的隱式轉換，讓錯誤在編譯時就被抓出來
 * 2. 強迫使用者明確表達意圖，程式碼更清楚
 * 3. 避免編譯器在 overload resolution 時選到意外的版本
 *
 *
 * --- 什麼時候該加？---
 *
 * 幾乎一定要加:
 *   - 單一參數 constructor（最常見的踩雷點）
 *   - 多參數但有預設值、變成「可單一參數」的 constructor
 *     例如: Foo(int a, int b = 0) 其實等同單一參數
 *
 * 不需要加:
 *   - 預設 constructor（無參數）
 *   - 真的明確多參數的 constructor
 *   - 你故意想要允許隱式轉換（罕見，例如 std::string 允許從 const char*）
 *
 *
 * --- C++11 起也可以加在 conversion operator 上 ---
 *
 *     class MyBool {
 *         explicit operator bool() const { return ...; }  // 只能明確轉 bool
 *     };
 *
 * 這樣可以避免 MyBool 被意外當成 int 使用。
 */
class Vector3 {
public:
    // 預設 constructor（沒有參數）— 不需要 explicit
    Vector3() : x_(0), y_(0), z_(0) {
        cout << "  [預設 ctor]" << endl;
    }

    // 帶參數的 constructor — 明確多參數，不需要 explicit
    Vector3(double x, double y, double z) : x_(x), y_(y), z_(z) {
        cout << "  [參數 ctor]" << endl;
    }

    // 單一參數 → 一定要加 explicit，防止意外的隱式轉換
    explicit Vector3(double scalar) : x_(scalar), y_(scalar), z_(scalar) {
        cout << "  [scalar ctor]" << endl;
    }

    // Delegating constructor（C++11）: 呼叫另一個 constructor
    explicit Vector3(const string& /*name*/) : Vector3(1, 2, 3) {
        cout << "  [delegating ctor]" << endl;
    }

    void print() const {
        cout << "  (" << x_ << ", " << y_ << ", " << z_ << ")" << endl;
    }

private:
    double x_, y_, z_;
};

// 示範 explicit 的差異
void explicit_comparison() {
    // Vector3 v = 5.0;  // 編譯錯誤！explicit 禁止隱式轉換
    Vector3 v(5.0);     // OK: 直接呼叫
    Vector3 w{5.0};     // OK: uniform initialization

    // 函式參數也一樣
    // auto f = [](Vector3) {};
    // f(5.0);            // 編譯錯誤（explicit）
    // f(Vector3(5.0));   // OK
    (void)v; (void)w;
}

void constructor_demo() {
    // Output:
    // === Constructor 形式 ===
    //   [預設 ctor]
    //   (0, 0, 0)
    //   [參數 ctor]
    //   (1, 2, 3)
    //   [scalar ctor]
    //   (5, 5, 5)
    //   [參數 ctor]
    //   [delegating ctor]
    //   (1, 2, 3)
    //
    cout << "=== Constructor 形式 ===" << endl;

    Vector3 v1;              // 預設 ctor
    v1.print();

    Vector3 v2(1.0, 2.0, 3.0);  // 參數 ctor
    v2.print();

    Vector3 v3(5.0);         // scalar ctor (explicit 只能明確呼叫)
    v3.print();

    Vector3 v4("origin");    // delegating ctor
    v4.print();

    cout << endl;
}

// ============================================================
// 4. 成員初始化列表 (member initializer list)
// ============================================================
class Person {
public:
    // 方式 A: 初始化列表（推薦）
    Person(const string& name, int age)
        : name_(name), age_(age) {   // 在 {} 之前就完成初始化
    }

    // 方式 B: 在 body 裡賦值（不推薦）
    // Person(const string& name, int age) {
    //     name_ = name;   // 先被預設建構，再被賦值 → 兩步
    //     age_ = age;
    // }

    void print() const {
        cout << "  " << name_ << " (" << age_ << " 歲)" << endl;
    }

private:
    string name_;
    int age_;
};

void initializer_list_demo() {
    // Output:
    // === 成員初始化列表 ===
    //   Bob (30 歲)
    //   (用初始化列表比在 body 賦值更有效率)
    //
    cout << "=== 成員初始化列表 ===" << endl;
    Person p("Bob", 30);
    p.print();
    cout << "  (用初始化列表比在 body 賦值更有效率)" << endl;
    cout << endl;
}

// ============================================================
// 5. Access Specifier 總整理：public / private / protected
// ============================================================
/**
 *   public    → 誰都能存取（外部使用者、子類別、自己）
 *   private   → 只有自己（class 內部）能存取
 *   protected → 自己 + 子類別能存取（見 2-3 Inheritance）
 *
 * 設計原則：預設 private，只把「使用者需要的 API」開成 public。
 * 資料越封閉越好，之後想改實作才不會影響使用者。
 */
class Encapsulated {
public:
    Encapsulated() : secret_(42) {}

    // public API：使用者透過這些函式操作
    int get_secret() const { return secret_; }
    void set_secret(int v) {
        if (v >= 0) secret_ = v;  // 可以加入驗證邏輯
    }

private:
    int secret_;  // 成員資料藏起來
};

void access_specifier_demo() {
    // Output:
    // === Access Specifier ===
    //   初始值: 42
    //   修改後: 100
    //
    cout << "=== Access Specifier ===" << endl;

    Encapsulated e;
    cout << "  初始值: " << e.get_secret() << endl;
    e.set_secret(100);
    cout << "  修改後: " << e.get_secret() << endl;

    // e.secret_ = -999;  // 編譯錯誤！private，而且也繞過了驗證邏輯
    cout << endl;
}

// ============================================================
// 6. const 成員函式（const method）/ mutable
// ============================================================
/**
 * 加在方法尾端的 const 代表「這個方法不會修改成員變數」。
 *
 * 規則：
 *   - const 方法不能修改任何非 mutable 的成員
 *   - const 方法只能呼叫其他 const 方法
 *   - const 物件 / const reference 只能呼叫 const 方法
 *
 * 實務：所有「只讀」的 getter 都應該加 const。
 *
 *
 * --- mutable 是什麼？---
 *
 * mutable 放在 member variable 上，意思是：
 *
 *     即使在 const method 裡，也允許修改這個 member。
 *
 * 這不是叫你偷改資料，而是用來處理「不改變物件對外邏輯狀態」的內部細節。
 *
 * 常見合理用途：
 *   - cache：第一次查詢時計算並記住結果
 *   - lazy initialization：需要時才建立內部輔助資料
 *   - debug / metrics counter：統計 getter 被呼叫幾次
 *   - mutex：const getter 也需要 lock/unlock，但 lock 不算改變物件邏輯值
 *
 * 不該用 mutable 的情況：
 *   - 只是想在 const method 裡修改真正資料
 *   - 用來繞過 const correctness
 *
 * 簡單判斷：
 *   對外看起來物件的值有沒有變？
 *   如果沒有，只是 cache/mutex/統計，mutable 才合理。
 */
class Counter {
public:
    Counter() : count_(0) {}

    void increment() { ++count_; }        // 修改 state → 不能 const

    int value() const { return count_; }  // 只讀 → 加 const
    // int value() const { ++count_; }    // 編譯錯誤！const 方法不能改成員

private:
    int count_;
};

class CachedCounterView {
public:
    explicit CachedCounterView(int count) : count_(count) {}

    string label() const {
        // label() 對外是只讀查詢：count_ 沒有變。
        // 但我們想 lazy 建立 cache，避免每次都重新組字串。
        //
        // cached_label_ 和 cache_ready_ 是內部加速細節，不是物件的邏輯狀態。
        // 所以它們可以是 mutable。
        if (!cache_ready_) {
            cached_label_ = "count = " + to_string(count_);
            cache_ready_ = true;
        }
        ++label_call_count_;
        return cached_label_;
    }

    int label_call_count() const {
        return label_call_count_;
    }

private:
    int count_;                         // 真正資料，const method 不應修改
    mutable bool cache_ready_ = false;  // 內部 cache 狀態，可在 const method 更新
    mutable string cached_label_;       // 內部 cache 內容，可在 const method 更新
    mutable int label_call_count_ = 0;  // debug/metrics，不改變物件邏輯值
};

void print_value(const Counter& c) {
    cout << "  (const ref) count: " << c.value() << endl;
    // c.increment();  // 編譯錯誤！const ref 只能呼叫 const 方法
}

void const_method_demo() {
    // Output:
    // === const 成員函式 / mutable ===
    //   count: 2
    //   const 物件 count: 0
    //   (const ref) count: 2
    //   mutable cache label: count = 5
    //   label 再查一次: count = 5
    //   label() 呼叫次數: 2
    //   注意：count_ 沒變，mutable 只用在 cache/metrics 這類內部細節
    //
    cout << "=== const 成員函式 / mutable ===" << endl;

    Counter c;
    c.increment();
    c.increment();
    cout << "  count: " << c.value() << endl;

    const Counter cc;  // const 物件
    // cc.increment();                   // 錯！const 物件不能呼叫 non-const
    cout << "  const 物件 count: " << cc.value() << endl;

    print_value(c);

    const CachedCounterView view(5);
    cout << "  mutable cache label: " << view.label() << endl;
    cout << "  label 再查一次: " << view.label() << endl;
    cout << "  label() 呼叫次數: " << view.label_call_count() << endl;
    cout << "  注意：count_ 沒變，mutable 只用在 cache/metrics 這類內部細節" << endl;
    cout << endl;
}

// ============================================================
// 7. static 成員（屬於 class，不屬於任何物件）
// ============================================================
/**
 * static 成員 = 屬於「class 本身」，所有物件共享。
 *
 * - static 變數：整個程式只有一份，不是每個物件各有一份
 * - static 方法：不能存取非 static 成員（因為沒有 this）
 *
 * 常見用途：
 *   - 計數（所有物件共用的 counter）
 *   - 常數（取代 #define）
 *   - 工廠方法（回傳新物件）
 *   - 工具方法（不需要物件狀態）
 */
class Widget {
public:
    Widget(const string& name) : name_(name) {
        ++count_;  // static 成員可以被任何方法修改
    }

    ~Widget() {
        --count_;
    }

    // 一般方法（instance method）：操作特定物件
    const string& name() const { return name_; }

    // static 方法（class method）：不屬於任何物件
    static int count() { return count_; }
    //              ^^ 注意：static 方法後面不能加 const
    //                 (因為 const 是約束 this，而 static 沒有 this)

    // static 常數
    static constexpr int MAX_COUNT = 100;

private:
    string name_;
    static int count_;  // 宣告（所有物件共享）
};

// static 成員變數需要在 class 外定義（分配空間）
int Widget::count_ = 0;

void static_demo() {
    // Output:
    // === static 成員 ===
    //   初始 count: 0
    //   MAX_COUNT: 100
    //   建立 w1, w2 後 count: 2
    //   建立 w3 後 count: 3
    //   w3 解構後 count: 2
    //   全部解構後 count: 0
    cout << "=== static 成員 ===" << endl;

    // 呼叫 static 方法不需要物件，用 ClassName::method() 的語法
    cout << "  初始 count: " << Widget::count() << endl;
    cout << "  MAX_COUNT: " << Widget::MAX_COUNT << endl;

    {
        Widget w1("A");
        Widget w2("B");
        cout << "  建立 w1, w2 後 count: " << Widget::count() << endl;

        {
            Widget w3("C");
            cout << "  建立 w3 後 count: " << Widget::count() << endl;
        }  // w3 解構

        cout << "  w3 解構後 count: " << Widget::count() << endl;
    }  // w1, w2 解構

    cout << "  全部解構後 count: " << Widget::count() << endl;
    cout << endl;
}

// ============================================================
// 8. 修飾詞組合速查表
// ============================================================
/**
 *              |  public  |  private | protected
 *   -----------+----------+----------+-----------
 *   外部       |   ✓      |   ✗      |   ✗
 *   子類別     |   ✓      |   ✗      |   ✓
 *   class 自己 |   ✓      |   ✓      |   ✓
 *
 *
 *   const method    → 承諾不改成員，const 物件可呼叫
 *   mutable member  → 允許 const method 修改該 member，常用於 cache/mutex/metrics
 *   static method   → 不屬於物件，沒有 this，不能改非 static 成員
 *   static member   → 所有物件共享，ClassName:: 存取
 *
 *
 *   常見組合:
 *   - public  const          → 對外只讀查詢 (getter)
 *   - public  static         → 工廠方法、工具函式
 *   - public  static const   → 常數
 *   - private                → 內部狀態/實作細節
 *   - private static         → 所有物件共享的內部狀態（如計數器）
 */

int main() {
    struct_demo();
    class_demo();
    constructor_demo();
    initializer_list_demo();
    access_specifier_demo();
    const_method_demo();
    static_demo();
    return 0;
}
