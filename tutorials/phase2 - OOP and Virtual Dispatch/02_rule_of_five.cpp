/**
 * Phase 2-2: Rule of Three / Five / Zero
 *
 * 當 class 管理資源（raw pointer、file handle、socket...）時，
 * 需要正確實作「特殊成員函式」(special member functions)。
 *
 * C++ 的五個特殊成員函式:
 *
 *   ~Class();                                // (1) Destructor          解構
 *   Class(const Class&);                     // (2) Copy constructor    用另一個物件來建構
 *   Class& operator=(const Class&);          // (3) Copy assignment     用另一個物件來賦值
 *   Class(Class&&) noexcept;                 // (4) Move constructor    從另一個物件「搬走」資源來建構
 *   Class& operator=(Class&&) noexcept;      // (5) Move assignment     從另一個物件「搬走」資源來賦值
 *
 *
 * --- Rule of Three (C++03) ---
 * 如果你自己實作了 (1)(2)(3) 之中的「任何一個」，通常三個都要實作。
 * 理由：會需要自訂 destructor 的 class 通常管理 raw 資源，
 *       編譯器預設的 copy 只是 shallow copy → double free。
 *
 * --- Rule of Five (C++11) ---
 * 進入 C++11 後，如果你想支援 move 語意（效能考量），
 * 要把 (1)(2)(3)(4)(5) 五個都實作好。
 * 注意：一旦你自己寫了 (1)~(3) 任何一個，編譯器就「不會」自動產生 (4)(5)，
 *       這時如果沒補上 move，類別就只有 copy，效能變差。
 *
 * --- Rule of Zero ---
 * 最推薦：讓你的 class 不直接管理 raw 資源，改用 smart pointer / STL 容器。
 * 這樣五個特殊成員函式「一個都不用寫」——編譯器產生的預設版本會自動做對事。
 *
 *
 * 選擇流程圖:
 *
 *   是否管理 raw 資源？
 *     │
 *     ├── 否 → Rule of Zero（什麼都不寫，組合 vector/unique_ptr 就對了）
 *     │
 *     └── 是 → 寫全 5 個（Rule of Five）
 *              只有獨佔資源 → copy ctor / copy assign 可以 = delete
 *                           （例如 unique_ptr 就是這樣設計的）
 *
 *
 * 目錄:
 *   1.   為什麼需要 Rule of Three（預設複製的 bug）
 *   2.   Rule of Three：手動實作 copy ctor / copy assignment
 *   3.   Rule of Five：加上 move 語意（為何需要）
 *        - T&& 是什麼？lvalue vs rvalue
 *        - Copy assign vs Move assign 實作差異
 *   3.5  noexcept 是什麼？為什麼 move 特別需要？
 *   4.   = default 與 = delete
 *   5.   Rule of Zero（最推薦的做法）
 *   6.   複製行為對照表
 */

#include <iostream>
#include <vector>
#include <utility>  // std::move
#include <algorithm>
using namespace std;

// ============================================================
// 1. 為什麼需要 Rule of Three
// ============================================================
/**
 * 下面這個 class 管理一塊動態記憶體。
 * 沒寫 copy ctor / copy assignment 時，編譯器會產生「逐成員複製」
 * 的版本 — 只複製 pointer（淺拷貝），造成 double free。
 *
 *   BadBuffer a(5);
 *   BadBuffer b = a;   // 淺拷貝：a.data_ 和 b.data_ 指向同一塊
 *   // 兩個 destructor 都 delete[] → double free → crash
 */

// ============================================================
// 2. Rule of Three：完整實作 copy
// ============================================================
class Buffer3 {
public:
    explicit Buffer3(size_t size) : size_(size), data_(new int[size]) {
        fill(data_, data_ + size_, 0);
        cout << "  [ctor] 配置 " << size_ << " 個 int" << endl;
    }

    // (1) Destructor
    ~Buffer3() {
        delete[] data_;
        cout << "  [dtor] 釋放" << endl;
    }

    // (2) Copy constructor：深拷貝
    Buffer3(const Buffer3& other) : size_(other.size_), data_(new int[other.size_]) {
        copy(other.data_, other.data_ + size_, data_);
        cout << "  [copy ctor] 深拷貝 " << size_ << " 個 int" << endl;
    }

    // (3) Copy assignment：先釋放舊的，再深拷貝
    Buffer3& operator=(const Buffer3& other) {
        if (this == &other) return *this;  // 防止 self-assignment

        delete[] data_;                    // 釋放舊資源
        size_ = other.size_;
        data_ = new int[size_];
        copy(other.data_, other.data_ + size_, data_);

        cout << "  [copy assign]" << endl;
        return *this;
    }

    int& operator[](size_t i) { return data_[i]; }
    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

void rule_of_three_demo() {
    // Output:
    // === Rule of Three ===
    //   [ctor] 配置 3 個 int
    //   [copy ctor] 深拷貝 3 個 int
    //   a[0] = 10, b[0] = 99
    //   [ctor] 配置 1 個 int
    //   [copy assign]
    //   c[0] = 10
    //
    //   [dtor] 釋放
    //   [dtor] 釋放
    //   [dtor] 釋放
    cout << "=== Rule of Three ===" << endl;

    Buffer3 a(3);
    a[0] = 10;

    Buffer3 b = a;       // 呼叫 copy ctor
    b[0] = 99;           // 改 b 不影響 a（深拷貝）

    cout << "  a[0] = " << a[0] << ", b[0] = " << b[0] << endl;

    Buffer3 c(1);
    c = a;               // 呼叫 copy assignment
    cout << "  c[0] = " << c[0] << endl;

    cout << endl;
}

// ============================================================
// 3. Rule of Five：加上 move 語意
// ============================================================
/**
 * 只有 copy 版本的話，下面這行會觸發**複製**（分配新記憶體 + memcpy）：
 *
 *   Buffer3 tmp = create_buffer();   // 理論上應該「搬走」，但會複製
 *
 * 加上 move ctor / move assignment 後，編譯器能把資源「轉移」而不是複製，
 * 對大物件來說省去大量記憶體分配。
 *
 *
 * --- T&& 是什麼？是「reference 的 reference」嗎？ ---
 *
 * 不是！&& 是 C++11 引入的「rvalue reference（右值引用）」，
 * 跟 & (lvalue reference) 是不同的型別。
 *
 *     int& lref = x;       // lvalue reference: 綁定有名字的變數
 *     int&& rref = 42;     // rvalue reference: 綁定暫時的、即將消失的值
 *
 * lvalue vs rvalue 怎麼區分？
 *   - lvalue：有名字、有位址、能再次使用 → x, v[0], *p
 *   - rvalue：暫時的、沒名字、即將消失 → 42, x+1, func() 的回傳值
 *
 *     int x = 10;
 *     int&  a = x;                 // OK: x 是 lvalue
 *     int&& b = x;                 // 錯誤！x 是 lvalue，不能綁 &&
 *     int&& c = 42;                // OK: 42 是 rvalue
 *     int&& d = std::move(x);      // OK: std::move(x) 把 x 標記為 rvalue
 *
 *
 * --- Copy assign vs Move assign 的參數為什麼不同？ ---
 *
 * 就是靠「參數型別」讓編譯器區分兩種不同意圖：
 *
 *   Copy assign:   operator=(const T& other)   ← 接受 lvalue
 *                  「我只看看，不會動你」
 *                  → 要分配新記憶體 + 深拷貝內容
 *
 *   Move assign:   operator=(T&& other)        ← 接受 rvalue
 *                  「你馬上要消失了，讓我搬走你的資源」
 *                  → 直接偷走指標 + 掏空對方（超快）
 *
 *
 * --- 編譯器如何挑選？---
 *
 *     Buffer5 a(5), b(5), c(5);
 *
 *     b = a;               // a 是 lvalue → 選 copy assign（複製 a）
 *     c = std::move(a);    // std::move(a) 是 rvalue → 選 move assign（掏空 a）
 *     c = Buffer5(10);     // Buffer5(10) 是暫時物件→ 選 move assign
 *
 *
 * --- 兩者的實作差異 ---
 *
 *   Copy assign (const T&)                  Move assign (T&&)
 *   --------------------------------        --------------------------------
 *   delete[] data_;                         delete[] data_;
 *   size_ = other.size_;                    size_ = other.size_;
 *   data_ = new int[size_];       ← 分配    data_ = other.data_;     ← 偷指標
 *   copy(other.data_, ..., data_); ← 複製   other.size_ = 0;         ← 掏空
 *                                           other.data_ = nullptr;
 *
 * 效能差異：複製 1GB 資料要搬 1GB；move 只改 2 個 pointer → 天差地遠。
 *
 *
 * --- 為什麼不能共用一個 arg？ ---
 *
 * 如果 copy 和 move 共用同一個版本，「能不能掏空 other」的行為會不明確：
 *
 *     Buffer5 a(5), b(5);
 *     b = a;   // ← 如果這行偷偷把 a 掏空，你之後用 a 就爆炸了
 *
 * 所以 C++ 透過「參數型別」區分兩種意圖，編譯器根據實際傳進來的東西
 * （lvalue 還是 rvalue）自動選對的版本。這就是 move semantics 的精髓。
 */
class Buffer5 {
public:
    explicit Buffer5(size_t size) : size_(size), data_(new int[size]) {
        fill(data_, data_ + size_, 0);
        cout << "  [ctor]" << endl;
    }

    ~Buffer5() {
        delete[] data_;
        cout << "  [dtor]" << endl;
    }

    // Copy ctor / assignment：深拷貝（同上）
    Buffer5(const Buffer5& other) : size_(other.size_), data_(new int[other.size_]) {
        copy(other.data_, other.data_ + size_, data_);
        cout << "  [copy ctor]" << endl;
    }

    Buffer5& operator=(const Buffer5& other) {
        if (this == &other) return *this;
        delete[] data_;
        size_ = other.size_;
        data_ = new int[size_];
        copy(other.data_, other.data_ + size_, data_);
        cout << "  [copy assign]" << endl;
        return *this;
    }

    // Move ctor：偷走對方的資源，把對方掏空
    // noexcept 很重要：STL 容器只會在 move 保證不 throw 時使用 move
    Buffer5(Buffer5&& other) noexcept
        : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;     // 掏空 → 讓對方的 dtor 不會 delete
        cout << "  [move ctor] 不分配新記憶體，只轉移指標" << endl;
    }

    // Move assignment
    Buffer5& operator=(Buffer5&& other) noexcept {
        if (this == &other) return *this;

        delete[] data_;             // 釋放自己原本的

        size_ = other.size_;        // 接收對方的
        data_ = other.data_;

        other.size_ = 0;            // 掏空對方
        other.data_ = nullptr;

        cout << "  [move assign]" << endl;
        return *this;
    }

    int& operator[](size_t i) { return data_[i]; }
    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

Buffer5 create_buffer() {
    Buffer5 b(5);
    b[0] = 42;
    return b;   // 回傳時可能觸發 move（或 RVO 直接省略）
}

void rule_of_five_demo() {
    // Output:
    // === Rule of Five ===
    //   建立 a:
    //   [ctor]
    //   a[0] = 42
    //   建立 b，用 std::move 轉移 a 的資源:
    //   [move ctor] 不分配新記憶體，只轉移指標
    //   b[0] = 42
    //   a.size() = 0 (被掏空)
    //
    //   [dtor]
    //   [dtor]
    cout << "=== Rule of Five ===" << endl;

    cout << "  建立 a:" << endl;
    Buffer5 a = create_buffer();
    cout << "  a[0] = " << a[0] << endl;

    cout << "  建立 b，用 std::move 轉移 a 的資源:" << endl;
    Buffer5 b = std::move(a);
    cout << "  b[0] = " << b[0] << endl;
    cout << "  a.size() = " << a.size() << " (被掏空)" << endl;

    cout << endl;
}

// ============================================================
// 3.5 noexcept 是什麼？為什麼 move 特別需要？
// ============================================================
/**
 * noexcept 是 C++11 關鍵字，標記「這個函式保證不會 throw exception」。
 *
 *     void foo() noexcept { ... }            // 保證不 throw
 *     Buffer(Buffer&&) noexcept { ... }      // move ctor 不 throw
 *
 * 注意：這是「承諾」，不是「檢查」。
 * 如果函式實際 throw 了，runtime 會直接呼叫 std::terminate() 讓程式死掉。
 *
 *
 * --- 為什麼 move 特別要加 noexcept？---
 *
 * STL 容器（例如 vector）在擴充容量時，要把舊元素搬到新的記憶體。
 * 它會「偷偷檢查」你的 move ctor 有沒有 noexcept：
 *
 *     move ctor 有 noexcept  → vector 用 move（快）
 *     move ctor 沒有 noexcept → vector 用 copy（慢）
 *
 * 為什麼？因為 vector 要保證「搬到一半 throw 也能回到原狀」：
 *
 *     Copy 中途 throw：沒事，舊元素還在，丟掉複製的就好。
 *     Move 中途 throw：糟糕，舊元素已經被掏空，回不去了。
 *
 * 所以 vector 只敢在「move 保證不 throw」時才用 move。
 * 你沒標 noexcept，你的 class 放進 vector 就會退化成 copy，效能輸光。
 *
 *
 * --- 標 noexcept 的好處 ---
 *
 * 1. 效能：STL 容器用 move 而非 copy（reallocate、insert、erase 時）
 * 2. 文件化：明確告訴使用者「這個操作不會失敗」
 * 3. 編譯器優化：編譯器知道不會 throw，可以省略 exception 相關程式碼
 * 4. 讓你的 class 能正確用在 noexcept 要求的地方
 *    （例如 std::swap、某些 STL 函式）
 *
 *
 * --- 哪些函式該加 noexcept？---
 *
 * 幾乎一定要加:
 *   - Move ctor、move assignment
 *   - Destructor（C++11 起預設就是 noexcept，不用特別寫）
 *   - swap 函式
 *   - 簡單的 getter
 *
 * 可以加的:
 *   - 不會 throw 的數學/邏輯運算
 *   - 只操作 POD（plain data）的函式
 *
 * 不要加:
 *   - 會配置記憶體的函式（new 可能 throw bad_alloc）
 *   - 呼叫其他可能 throw 的函式
 *   - I/O 操作
 *
 *
 * --- 驗證範例 ---
 *
 *   class A {
 *       vector<int> v_;
 *   public:
 *       A(A&& other) noexcept : v_(std::move(other.v_)) {}  // vector 的 move 也 noexcept
 *   };
 *
 * 可以用 static_assert 檢查:
 *   static_assert(std::is_nothrow_move_constructible_v<A>,
 *                 "A 的 move ctor 應該是 noexcept");
 */
#include <type_traits>
static_assert(std::is_nothrow_move_constructible_v<Buffer5>,
              "Buffer5 的 move ctor 應該是 noexcept");

// ============================================================
// 4. = default 與 = delete
// ============================================================
/**
 * = default: 明確要求編譯器產生預設版本
 * = delete:  明確禁止某個操作
 */
class Defaulted {
public:
    Defaulted() = default;                         // 用預設 ctor
    Defaulted(const Defaulted&) = default;         // 用預設 copy ctor
    Defaulted& operator=(const Defaulted&) = default;
    Defaulted(Defaulted&&) = default;
    Defaulted& operator=(Defaulted&&) = default;
    ~Defaulted() = default;
};

class NonCopyable {
public:
    NonCopyable() = default;

    // 禁止複製（例如管理獨佔資源時）
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    // 通常還是允許 move
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

void default_delete_demo() {
    // Output:
    // === = default / = delete ===
    //   (範例：unique_ptr 就是 non-copyable 的設計)
    //
    cout << "=== = default / = delete ===" << endl;

    NonCopyable a;
    // NonCopyable b = a;   // 編譯錯誤！被 delete
    NonCopyable c = std::move(a);  // OK
    (void)c;

    cout << "  (範例：unique_ptr 就是 non-copyable 的設計)" << endl;
    cout << endl;
}

// ============================================================
// 5. Rule of Zero：讓 smart pointer / STL 幫你處理
// ============================================================
/**
 * 最推薦的做法：不管理 raw 資源，改用 smart pointer / STL container。
 * 這樣編譯器產生的預設 copy/move 就會「做對事」——因為 vector、string、
 * unique_ptr 都已經正確實作了自己的 Rule of Five。
 *
 * 結果：你的 class 一行特殊成員函式都不用寫。
 *
 * 延伸：Rule of Zero 的 class 若 constructor 要收 string / vector 這類參數存進成員，
 * 預設寫法是 by-value 再 std::move：
 *     explicit User(string name) : name_(std::move(name)) {}
 * 呼叫端傳 lvalue 是一次 copy 加一次便宜的 move，傳 rvalue 則完全不 copy。
 * 為什麼這樣選、什麼時候該改成 const T& 加 T&& 兩個 overload，見 phase5 01_move_semantics 第 8 章。
 */
class Buffer0 {
public:
    explicit Buffer0(size_t size) : data_(size, 0) {}

    int& operator[](size_t i) { return data_[i]; }
    size_t size() const { return data_.size(); }

    // 不需要 destructor、copy、move — 編譯器產生的預設版本就正確
    // （因為 vector 自己會處理深拷貝、move 等行為）

private:
    vector<int> data_;
};

void rule_of_zero_demo() {
    // Output:
    // === Rule of Zero ===
    //   a[0] = 10, b[0] = 99
    //   c[0] = 10
    //   (完全不用寫任何特殊成員函式，最乾淨的設計)
    cout << "=== Rule of Zero ===" << endl;

    Buffer0 a(3);
    a[0] = 10;

    Buffer0 b = a;           // 預設 copy ctor 自動做對事（vector 深拷貝）
    b[0] = 99;
    cout << "  a[0] = " << a[0] << ", b[0] = " << b[0] << endl;

    Buffer0 c = std::move(a); // 預設 move ctor 自動做對事（vector 轉移）
    cout << "  c[0] = " << c[0] << endl;

    cout << "  (完全不用寫任何特殊成員函式，最乾淨的設計)" << endl;
    cout << endl;
}

// ============================================================
// 6. 複製行為對照表
// ============================================================
/**
 *    宣告                               行為
 *    ---------------------------------- ----------------------------------
 *    不寫任何特殊成員                   編譯器產生所有預設版本
 *    寫了 destructor                   仍產生 copy，但不產生 move（危險）
 *    寫了 copy ctor 或 copy assign     必須把所有特殊成員都寫好（Rule of Three）
 *    = default                         明確使用預設版本
 *    = delete                          禁止此操作
 *
 *    最佳實踐：
 *    - 優先用 Rule of Zero（組合 vector / unique_ptr / shared_ptr）
 *    - 如果非要管理 raw 資源，完整實作 Rule of Five
 *    - 半套實作（只寫 destructor）是最糟糕的情況
 */

int main() {
    rule_of_three_demo();
    rule_of_five_demo();
    default_delete_demo();
    rule_of_zero_demo();
    return 0;
}
