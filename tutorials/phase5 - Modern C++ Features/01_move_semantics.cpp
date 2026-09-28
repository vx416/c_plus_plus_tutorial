/**
 * Phase 5-1: Move Semantics 深入
 *
 * Move semantics 是 C++ 用來避免不必要深拷貝的機制。
 * 它的核心不是「把記憶體搬來搬去」，而是「把資源所有權轉移」。
 *
 * 目錄:
 *   1. lvalue vs rvalue
 *   2. 完整的 value category：lvalue / xvalue / prvalue
 *   3. std::move 的真正意思
 *   4. move constructor / move assignment
 *   5. return 時的 copy elision：RVO / NRVO / implicit move
 *   6. noexcept 與 STL container
 *   7. std::forward 與 forwarding reference
 *   8. 參數怎麼收：observer / in-out / sink
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// ============================================================
// 1. lvalue vs rvalue
// ============================================================
// 本章重點：
//   lvalue 通常是有名字、可以在這行之後繼續使用的物件。
//   rvalue 通常是暫時值，快要消失，所以資源可以被搬走。
//
//   string s = "hello"; 中 s 是 lvalue。
//   string("temp") 是 rvalue。
void take_lvalue(const string& value) {
    cout << "  const lvalue ref: " << value << endl;
}

void take_rvalue(string&& value) {
    cout << "  rvalue ref: " << value << endl;
}

void value_category_demo() {
    // Output:
    // === lvalue vs rvalue ===
    //   const lvalue ref: Alice
    //   const lvalue ref: temporary
    //   rvalue ref: temporary
    //
    cout << "=== lvalue vs rvalue ===" << endl;

    string name = "Alice";
    take_lvalue(name);
    take_lvalue(string("temporary"));
    take_rvalue(string("temporary"));
    // take_rvalue(name); // 編譯錯誤：name 是 lvalue，不能綁定到 string&&
    cout << endl;
}

// ============================================================
// 2. 完整的 value category：lvalue / xvalue / prvalue
// ============================================================
// 本章重點：
//   「lvalue vs rvalue」是簡化說法。C++11 起每個 expression 其實分成三種：
//
//                 有身分 (identity)   可以被搬走 (movable)
//     lvalue          有                  否            x, *p, s.name, f() 回傳 T&
//     xvalue          有                  是            std::move(x), f() 回傳 T&&
//     prvalue         無                  是            42, string("tmp"), a + b, f() 回傳 T
//
//   兩個集合名稱：
//     glvalue = lvalue + xvalue   （有身分：可以拿位址、可以觀察它是「哪一個」物件）
//     rvalue  = xvalue + prvalue  （可以被搬走：能綁到 T&&，會選到 move ctor）
//
//   所以 std::move(x) 的回傳值是 xvalue，不是 prvalue：
//   它仍然是 x 那個物件（有身分），只是被標記成「可以搬」。
//
//   常見陷阱：有名字的 rvalue reference 本身是 lvalue。
//     string&& r = string("tmp");
//     r 是 lvalue，傳給函式時會選到 const string& 版本，不會自動 move。
//     這就是為什麼 move constructor 裡面還要對成員再寫一次 std::move。
//
//   C++17 補充：prvalue 不再直接代表一個暫時物件。
//     只有在需要時（例如綁到 reference、取成員）才會「物化」成暫時物件，
//     這叫 temporary materialization，也是 guaranteed copy elision 的基礎：
//     string s = string("tmp"); 不會產生第二個物件。
//
//   底下用 decltype((expr)) 判斷分類，這是一個固定規則：
//     expr 是 lvalue  -> decltype((expr)) 是 T&
//     expr 是 xvalue  -> decltype((expr)) 是 T&&
//     expr 是 prvalue -> decltype((expr)) 是 T
template <typename T>
struct value_category { static constexpr const char* name = "prvalue"; };
template <typename T>
struct value_category<T&> { static constexpr const char* name = "lvalue"; };
template <typename T>
struct value_category<T&&> { static constexpr const char* name = "xvalue"; };

void sink(const string& value);
void sink(string&& value);

#define CATEGORY_OF(expr) value_category<decltype((expr))>::name

string make_string() { return "prvalue"; }
string& get_ref(string& s) { return s; }
string&& get_rref(string& s) { return std::move(s); }

void full_value_category_demo() {
    // Output:
    // === lvalue / xvalue / prvalue ===
    //   x                      -> lvalue
    //   42                     -> prvalue
    //   string("tmp")          -> prvalue
    //   x + "!"                -> prvalue
    //   std::move(x)           -> xvalue
    //   static_cast<string&&>(x) -> xvalue
    //   named rvalue ref r     -> lvalue
    //   std::move(r)           -> xvalue
    //   make_string()          -> prvalue
    //   get_ref(x)             -> lvalue
    //   get_rref(x)            -> xvalue
    //   pair.first             -> lvalue
    //   std::move(pair).first  -> xvalue
    //   sink(r):    sink const&: named rvalue ref is lvalue
    //   sink(std::move(r)):    sink &&: named rvalue ref is lvalue
    //
    cout << "=== lvalue / xvalue / prvalue ===" << endl;

    string x = "x";
    cout << "  x                      -> " << CATEGORY_OF(x) << endl;
    cout << "  42                     -> " << CATEGORY_OF(42) << endl;
    cout << "  string(\"tmp\")          -> " << CATEGORY_OF(string("tmp")) << endl;
    cout << "  x + \"!\"                -> " << CATEGORY_OF(x + "!") << endl;
    cout << "  std::move(x)           -> " << CATEGORY_OF(std::move(x)) << endl;
    cout << "  static_cast<string&&>(x) -> " << CATEGORY_OF(static_cast<string&&>(x)) << endl;

    string&& r = string("named rvalue ref is lvalue");
    cout << "  named rvalue ref r     -> " << CATEGORY_OF(r) << endl;
    cout << "  std::move(r)           -> " << CATEGORY_OF(std::move(r)) << endl;

    cout << "  make_string()          -> " << CATEGORY_OF(make_string()) << endl;
    cout << "  get_ref(x)             -> " << CATEGORY_OF(get_ref(x)) << endl;
    cout << "  get_rref(x)            -> " << CATEGORY_OF(get_rref(x)) << endl;

    pair<string, int> p{"first", 1};
    cout << "  pair.first             -> " << CATEGORY_OF(p.first) << endl;
    cout << "  std::move(pair).first  -> " << CATEGORY_OF(std::move(p).first) << endl;

    // 陷阱實際跑一次：r 有名字，所以是 lvalue，overload 會選到 const&。
    cout << "  sink(r):  ";
    sink(r);
    cout << "  sink(std::move(r)):  ";
    sink(std::move(r));
    cout << endl;
}

// ============================================================
// 3. std::move 的真正意思
// ============================================================
// 本章重點：
//   std::move 不會搬任何東西。
//   它只做一件事：把 expression 轉成 rvalue，讓 move constructor/assignment 有機會被選到。
//
//   真正搬資源的是 string/vector/unique_ptr 等型別自己的 move 實作。
void std_move_demo() {
    // Output:
    // === std::move ===
    //   target = hello
    //   source is valid but unspecified after move, size = 0
    //
    cout << "=== std::move ===" << endl;

    string source = "hello";
    string target = std::move(source);

    cout << "  target = " << target << endl;
    cout << "  source is valid but unspecified after move, size = " << source.size() << endl;
    cout << endl;
}

// ============================================================
// 4. move constructor / move assignment
// ============================================================
// 本章重點：
//   擁有 raw resource 的 class 要自己定義 copy/move 行為。
//   move constructor 從另一個物件偷走資源，然後把來源物件放到可解構狀態。
class Buffer {
public:
    explicit Buffer(size_t size) : size_(size), data_(make_unique<int[]>(size)) {
        cout << "  Buffer ctor size=" << size_ << endl;
    }

    Buffer(const Buffer& other) : Buffer(other.size_) {
        copy(other.data_.get(), other.data_.get() + other.size_, data_.get());
        cout << "  Buffer copy ctor" << endl;
    }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) {
            return *this;
        }
        Buffer temp(other);
        swap(temp);
        cout << "  Buffer copy assignment" << endl;
        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : size_(other.size_), data_(std::move(other.data_)) {
        other.size_ = 0;
        cout << "  Buffer move ctor" << endl;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        size_ = other.size_;
        data_ = std::move(other.data_);
        other.size_ = 0;
        cout << "  Buffer move assignment" << endl;
        return *this;
    }

    size_t size() const { return size_; }

private:
    void swap(Buffer& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    size_t size_ = 0;
    unique_ptr<int[]> data_;
};

void move_constructor_demo() {
    // Output:
    // === move constructor / assignment ===
    //   Buffer ctor size=4
    //   Buffer ctor size=4
    //   Buffer copy ctor
    //   Buffer move ctor
    //   moved buffer size = 4
    //
    cout << "=== move constructor / assignment ===" << endl;

    Buffer a(4);
    Buffer b = a;
    Buffer c = std::move(a);
    cout << "  moved buffer size = " << c.size() << endl;
    cout << endl;
}

// ============================================================
// 5. return 時的 copy elision：RVO / NRVO / implicit move
// ============================================================
// 本章重點：
//   很多人以為「回傳物件 = 至少 move 一次」。實際上大多數情況連 move 都沒有。
//
//   (1) return prvalue（例如 return Buffer(4); 或 return make_string();）
//       傳統上叫 RVO（Return Value Optimization）。
//       C++17 起是「guaranteed copy elision」：語言保證直接在呼叫端的位置建構，
//       不 copy、不 move。這不是最佳化，是語意，所以 copy/move 都 delete 的型別也能回傳。
//       原理就是第 2 章講的 temporary materialization：prvalue 只是「怎麼初始化」的描述，
//       直到需要一個物件時才落地，而落地的位置就是呼叫端的變數。
//
//       C++17 之前沒有這個保證，會有三個問題：
//       a. 省略只是「允許」，但語意上 copy/move ctor 仍然被視為有呼叫，
//          所以它必須存在且可存取。copy/move 都 delete 的型別（mutex、atomic、
//          底下的 Pinned）連 return T{} 或 T x = T{}; 都無法編譯，
//          只能改用 unique_ptr<T> 包起來、或用 out-parameter 傳進去給函式填。
//       b. 省不省由編譯器決定，Debug build 或不同 compiler 可能真的多跑一次 move，
//          如果 ctor 有 side effect（例如印 log、計數），行為會隨編譯選項改變。
//       c. 就算實務上都省掉了，寫 library 的人也不能依賴它，
//          因為標準沒說一定省，跨平台就沒有承諾。
//       C++17 把 (a) 直接解決：既然語意上就沒有 copy/move，自然不需要那兩個 ctor 存在。
//       (b)(c) 對 prvalue 也一併解決；但對具名 local（NRVO）仍然只是允許，見 (2)。
//
//   (2) return 具名 local（Buffer b(5); return b;）
//       叫 NRVO（Named Return Value Optimization）。編譯器通常會把 b 直接建在呼叫端，
//       但標準只「允許」不「保證」（g++ 可用 -fno-elide-constructors 關掉來觀察差異）。
//       NRVO 做不到時（例如不同分支回傳不同 local），會退而求其次：
//       把 b 當成 rvalue 來選 constructor，這叫 implicit move。所以最差也是 move，不會 copy。
//
//   (3) return std::move(local) 是反效果
//       std::move(b) 是 xvalue 不是具名 local，NRVO 的條件不成立，
//       原本可以零成本的地方硬變成一次 move。compiler 會用 -Wpessimizing-move 警告。
//
//   (4) 函式參數 return 時 NRVO 不適用（參數的記憶體由呼叫端配置），但 implicit move 適用，
//       所以 return param; 是一次 move。
//
//   簡單記法：直接 return，讓編譯器決定。只有在回傳「不是 local 變數」的東西
//   （例如成員、reference 參數）而且確定要搬走時，才寫 std::move。
Buffer make_prvalue() {
    return Buffer(4);               // guaranteed elision：只會看到一次 ctor
}

Buffer make_named() {
    Buffer b(5);
    return b;                       // NRVO：通常也只會看到一次 ctor
}

Buffer make_named_with_move() {
    Buffer b(6);
    return std::move(b);            // 反效果：NRVO 失效，多一次 move ctor
}                                   // 編譯這個檔案時看到的 -Wpessimizing-move 警告就是它，故意留著讓你看到

Buffer make_from_branch(bool flag) {
    Buffer a(7);
    Buffer b(8);
    if (flag) {
        return a;                   // 兩個候選 local，NRVO 做不到
    }
    return b;                       // 退成 implicit move：一次 move ctor
}

Buffer pass_through(Buffer param) {
    return param;                   // 參數不能 NRVO，但 implicit move 適用：一次 move ctor
}

// copy 和 move 都 delete，仍然可以用 prvalue 回傳與初始化：證明這不是「省掉的 move」，而是根本沒有 move。
struct Pinned {
    Pinned() { cout << "  Pinned ctor" << endl; }
    Pinned(const Pinned&) = delete;
    Pinned(Pinned&&) = delete;
};

Pinned make_pinned() {
    return Pinned{};
}

void copy_elision_demo() {
    // Output:
    // === return / copy elision ===
    //   [prvalue]
    //   Buffer ctor size=4
    //   [named local, NRVO]
    //   Buffer ctor size=5
    //   [return std::move(local)]
    //   Buffer ctor size=6
    //   Buffer move ctor
    //   [branch, implicit move]
    //   Buffer ctor size=7
    //   Buffer ctor size=8
    //   Buffer move ctor
    //   [parameter, implicit move]
    //   Buffer ctor size=9
    //   Buffer move ctor
    //   [deleted copy & move]
    //   Pinned ctor
    //
    cout << "=== return / copy elision ===" << endl;

    cout << "  [prvalue]" << endl;
    Buffer p = make_prvalue();

    cout << "  [named local, NRVO]" << endl;
    Buffer n = make_named();

    cout << "  [return std::move(local)]" << endl;
    Buffer m = make_named_with_move();

    cout << "  [branch, implicit move]" << endl;
    Buffer br = make_from_branch(true);

    cout << "  [parameter, implicit move]" << endl;
    Buffer pt = pass_through(Buffer(9));   // Buffer(9) 是 prvalue，直接建在 param 上，不另外 move

    cout << "  [deleted copy & move]" << endl;
    Pinned pinned = make_pinned();

    (void)p; (void)n; (void)m; (void)br; (void)pt; (void)pinned;
    cout << endl;
}

// ============================================================
// 6. noexcept 與 STL container
// ============================================================
// 本章重點：
//   vector 擴容時需要把舊元素搬到新記憶體。
//   如果 move constructor 是 noexcept，vector 比較敢用 move。
//   如果 move 可能丟 exception，vector 可能退回 copy 以維持 exception safety。
void noexcept_demo() {
    // Output:
    // === noexcept and containers ===
    //   Buffer ctor size=2
    //   Buffer ctor size=3
    //   vector size = 2
    //
    cout << "=== noexcept and containers ===" << endl;

    vector<Buffer> buffers;
    buffers.reserve(2);
    buffers.emplace_back(2);
    buffers.emplace_back(3);
    cout << "  vector size = " << buffers.size() << endl;
    cout << endl;
}

// ============================================================
// 7. std::forward 與 forwarding reference
// ============================================================
// 本章重點：
//   function template 裡的 T&& 若 T 會被推導，叫 forwarding reference。
//   它可以接 lvalue，也可以接 rvalue。角色是「中間人」：自己不看不留，原樣轉交給下一層，
//   讓下一層決定要當 observer 還是 sink（角色分類見第 8 章）。
//
//   什麼才算 forwarding reference：只有「T 正在這次呼叫被推導」的 T&&。
//     template <typename T> void f(T&& v);          是
//     auto&& v = expr;                              是（auto 版本）
//     void f(string&& v);                           不是，就是 rvalue reference
//     template <typename T> void f(vector<T>&& v);  不是，推導的是 T 不是整個參數型別
//     template <typename T> struct Box { void put(T&& v); };   不是，T 在 class 層就固定了
//
//   它是怎麼同時接兩種的：reference collapsing。
//     傳入            T 推導成     T&& 折疊成     std::forward<T> 的結果
//     lvalue string   string&      string&        lvalue
//     rvalue string   string       string&&       rvalue（xvalue）
//   規則只有一條：& 跟任何東西折疊都是 &，只有 && 跟 && 才是 &&。
//
//   std::forward<T>(value) 會保留原本傳進來的 value category。
//   std::move(value) 則是不管原本是什麼，都轉成 rvalue。
//
//   坑：參數 value 有名字，所以在函式裡它永遠是 lvalue（第 2 章講過具名 rvalue reference 是 lvalue）。
//   忘了包 std::forward<T>，不管呼叫端傳什麼，下一層都會收到 lvalue，底下 forward_to_sink_without_forward 示範。
//   更多坑（只能 forward 一次、貪心 overload 搶走 copy ctor）見 phase3 03_variadic_templates 第 4 章。
void sink(const string& value) {
    cout << "  sink const&: " << value << endl;
}

void sink(string&& value) {
    cout << "  sink &&: " << value << endl;
}

template <typename T>
void forward_to_sink(T&& value) {
    sink(std::forward<T>(value));
}

template <typename T>
void forward_to_sink_without_forward(T&& value) {
    sink(value);   // value 有名字就是 lvalue，永遠選到 const& 版本
}

void forwarding_demo() {
    // Output:
    // === std::forward ===
    //   sink const&: lvalue
    //   sink &&: rvalue
    //   without std::forward:
    //   sink const&: lvalue
    //   sink const&: rvalue
    cout << "=== std::forward ===" << endl;

    string text = "lvalue";
    forward_to_sink(text);
    forward_to_sink(string("rvalue"));

    cout << "  without std::forward:" << endl;
    forward_to_sink_without_forward(text);
    forward_to_sink_without_forward(string("rvalue"));   // 明明傳 rvalue，下一層卻收到 lvalue
    cout << endl;
}

// ============================================================
// 8. 參數怎麼收：observer / in-out / sink
// ============================================================
// 本章重點：
//   先問「函式對這個參數要做什麼」，再決定型別：
//
//     角色       函式做什麼                寫法                        copy 次數
//     observer   只讀，不保存              string_view / const T&      0
//     in-out     修改呼叫端的物件          T&                          0
//     sink       存進成員、放進容器        T by-value 再 std::move     見下表
//     forward    不看不留，原樣轉交下一層  T&& 加 std::forward<T>      0（第 7 章）
//
//   sink parameter 的意思是「這個東西交給我了」：函式會把它據為己有，呼叫端拿不回來。
//   constructor、setter、push_back 的參數都是 sink。
//
//   sink 為什麼預設 by-value + move（Core Guidelines F.16）：
//
//     呼叫端傳    by-value + move        const T& + copy
//     lvalue      1 copy + 1 move        1 copy
//     rvalue      0 copy + 1~2 move      1 copy
//
//   lvalue 那一次 copy 是 sink 任務本身的成本：呼叫端要保留它的那份、你也要有自己的一份，
//   總得有人把資料複製出來，兩種寫法都躲不掉。所以差別只剩：
//     by-value 對 lvalue 多一次便宜的 move（string 是三個 word），
//     對 rvalue 省一次昂貴的 copy（heap 配置加整段複製），
//     而且一個簽名就能吃兩種呼叫，不用寫 overload。
//
//   例外，該改用 const T& 加 T&& 兩個 overload 的情況：
//   a. T 的 move 不便宜（std::array、全是數值成員的大 struct）。
//      move 幾乎等於 copy，lvalue 路徑會變成兩次真複製。
//   b. 重複呼叫的 setter，而且成員已經有 buffer。
//      const T& 走 copy assignment，capacity 夠就直接抄進舊 buffer，零配置零釋放；
//      by-value 走 move assignment，每次都是「參數配置一塊新的、成員釋放一塊舊的」。
//      constructor 沒有舊 buffer 可以重用，所以這條只對 setter 成立。
//
//   小型 trivially copyable 型別（int、指標、string_view、span，16 bytes 以內）一律 by-value。
//   ABI 會直接放 register，比 const T& 少一次間接存取，也沒有 aliasing 問題。
//   有非 trivial 的 dtor 或 copy/move ctor 的型別（string、unique_ptr）不管多小都走記憶體。
struct Tracked {
    string value;
    explicit Tracked(string v) : value(std::move(v)) {}
    Tracked(const Tracked& other) : value(other.value) { cout << "    Tracked copy" << endl; }
    Tracked(Tracked&& other) noexcept : value(std::move(other.value)) { cout << "    Tracked move" << endl; }
};

struct HoldByValue {
    explicit HoldByValue(Tracked t) : t_(std::move(t)) {}   // sink：by-value 再 move
    Tracked t_;
};

struct HoldByRef {
    explicit HoldByRef(const Tracked& t) : t_(t) {}          // const& 再 copy
    Tracked t_;
};

class Profile {
public:
    void set_by_ref(const string& s) { name_ = s; }            // copy assignment：capacity 夠就重用 buffer
    void set_by_value(string s) { name_ = std::move(s); }      // move assignment：接管 s 的 buffer，丟掉舊的
    const void* buffer() const { return name_.data(); }

private:
    string name_;
};

void parameter_passing_demo() {
    // Output:
    // === parameter passing: sink ===
    //   HoldByValue(lvalue):
    //     Tracked copy
    //     Tracked move
    //   HoldByValue(rvalue):
    //     Tracked move
    //   HoldByRef(lvalue):
    //     Tracked copy
    //   HoldByRef(rvalue):
    //     Tracked copy
    //   setter const&:   buffer reused = true
    //   setter by-value: buffer reused = false
    //
    cout << "=== parameter passing: sink ===" << endl;

    Tracked source("source");
    cout << "  HoldByValue(lvalue):" << endl;
    HoldByValue a(source);                 // copy 進參數，move 進成員
    cout << "  HoldByValue(rvalue):" << endl;
    HoldByValue b(Tracked("temp"));        // prvalue 直接建在參數上（elision），再 move 進成員
    cout << "  HoldByRef(lvalue):" << endl;
    HoldByRef c(source);                   // 綁 reference，copy 進成員
    cout << "  HoldByRef(rvalue):" << endl;
    HoldByRef d(Tracked("temp"));          // const& 不能搬，還是 copy

    // 字串要超過 SSO 長度，name_ 才會有 heap buffer 可以觀察
    const string long_a(40, 'a');
    const string long_b(40, 'b');
    Profile profile;
    profile.set_by_ref(long_a);
    const void* before = profile.buffer();
    profile.set_by_ref(long_b);
    cout << "  setter const&:   buffer reused = " << boolalpha << (profile.buffer() == before) << endl;
    before = profile.buffer();
    profile.set_by_value(long_a);
    cout << "  setter by-value: buffer reused = " << (profile.buffer() == before) << endl;

    (void)a; (void)b; (void)c; (void)d;
    cout << endl;
}

int main() {
    value_category_demo();
    full_value_category_demo();
    std_move_demo();
    move_constructor_demo();
    copy_elision_demo();
    noexcept_demo();
    forwarding_demo();
    parameter_passing_demo();
    return 0;
}
