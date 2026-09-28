/**
 * Phase 1-5: Smart Pointers
 *
 * Smart pointer 不是什麼魔法，本質上就是一個包了 raw pointer 的 RAII class，
 * 在 destructor 裡自動 delete。概念上等同於：
 *
 *   template<typename T>
 *   class unique_ptr {
 *       T* ptr_;
 *   public:
 *       unique_ptr(T* p) : ptr_(p) {}
 *       ~unique_ptr() { delete ptr_; }   // 離開 scope 自動釋放
 *       T& operator*() { return *ptr_; } // 解參考
 *       T* operator->() { return ptr_; } // 存取成員
 *   };
 *
 * 標準庫提供三種，不用自己寫：
 *   - unique_ptr: 獨佔所有權，不能複製，只能移動
 *   - shared_ptr: 共享所有權，reference counting
 *   - weak_ptr:   觀察 shared_ptr 但不影響 ref count
 *
 * 搭配 helper function 建立（避免直接寫 new）：
 *   - make_unique<T>(args...)  → 回傳 unique_ptr<T>  (C++14)
 *   - make_shared<T>(args...)  → 回傳 shared_ptr<T>
 *
 * 目錄:
 *   1.   unique_ptr — 獨佔所有權
 *        std::move vs .get() 的對比
 *   1.5  為什麼 linked list 需要 std::move（push_front 拆解）
 *   2.   unique_ptr 與函式（工廠模式、借用）
 *   3.   shared_ptr — 共享所有權、reference counting
 *   4.   weak_ptr — 打破循環引用（雙向 linked list 範例）
 *   5.   unique_ptr 管理陣列
 *   6.   Smart Pointer 選擇指南
 */

#include <iostream>
#include <memory>
using namespace std;

struct Resource
{
    string name;

    explicit Resource(const string &n) : name(n)
    {
        cout << "  [Resource 建構] " << name << endl;
    }

    ~Resource()
    {
        cout << "  [Resource 解構] " << name << endl;
    }

    void greet() const
    {
        cout << "  Hello from " << name << endl;
    }
};

// ============================================================
// 1. unique_ptr — 獨佔所有權
// ============================================================
void unique_ptr_demo()
{
    // Output:
    // === unique_ptr ===
    //   [Resource 建構] A
    //   Hello from A
    //   移動後 p1 是否為空: 是
    //   移動後 p2 接手資源:Yes
    //   Hello from A
    //   .get() 借用 raw pointer:   Hello from A
    //   p2 是否還是 owner: 是  (注意：borrowed 不能 delete，p2 才會自動清理)
    //   Hello from A
    //   即將離開 scope...
    //
    //   [Resource 建構] B
    //   Hello from B
    //   即將離開 scope...
    //   [Resource 解構] B
    //   [Resource 解構] A
    cout << "=== unique_ptr ===" << endl;

    // 用 make_unique 建立（C++14），比直接 new 更安全
    auto p1 = make_unique<Resource>("A");
    p1->greet();

    // ---- std::move 是什麼？----
    //
    // unique_ptr 是「獨佔所有權」，同一塊記憶體只能有一個 owner。
    // 所以它禁止複製：
    //
    //     auto p2 = p1;   // 編譯錯誤！
    //
    // 為什麼不能複製？如果可以複製，p1 和 p2 會指向同一塊記憶體，
    // 兩個都會在 destructor 裡 delete → double free → 程式爆炸。
    //
    // 那要怎麼把所有權從 p1 轉給 p2 呢？用 std::move：
    //
    //     auto p2 = std::move(p1);
    //
    // std::move(p1) 的意思是「我不再需要 p1 了，把它的內容搬走」。
    // 執行後：
    //     p1 → nullptr       (被掏空)
    //     p2 → Resource-A    (接手所有權)
    //
    // 注意：std::move 本身不會真的搬動任何東西，它只是一個「標記」，
    // 告訴編譯器「這個變數可以被掏空」，實際的搬移動作是 unique_ptr
    // 的 move constructor 做的（內部就是把 raw pointer 轉移過來，
    // 然後把原本的設成 nullptr）。

    auto p2 = std::move(p1);
    cout << "  移動後 p1 是否為空: " << (p1 == nullptr ? "是" : "否") << endl;
    cout << "  移動後 p2 接手資源:" << (p2 == nullptr ? "No" : "Yes") << endl;
    p2->greet();

    // ---- .get() vs std::move ----
    //
    // unique_ptr 有兩種「取出內部 raw pointer」的方式，用途完全不同：
    //
    //   .get()       → 借用 raw pointer，不轉移所有權
    //                   unique_ptr 依然是 owner，負責 delete
    //
    //   std::move()  → 轉移所有權給別人
    //                   原本的 unique_ptr 變成 nullptr，不再負責 delete
    //
    // 對照表：
    //
    //   操作                    p2 的狀態    新指標的身份
    //   ---------------------   ----------   ----------------------
    //   Resource* r = p2.get(); 還是 owner   只是借來看，不能 delete
    //   auto p3 = std::move(p2); 變 nullptr  p3 變成新 owner
    //
    // 常見場景:
    //   - .get()     → 傳給只需要「用一下」的函式（API 要求 raw pointer）
    //   - std::move  → 要把資源「交出去」給別人管（塞進容器、存成成員）
    {
        Resource *borrowed = p2.get();
        cout << "  .get() 借用 raw pointer: ";
        borrowed->greet();
        cout << "  p2 是否還是 owner: " << (p2 != nullptr ? "是" : "否")
             << "  (注意：borrowed 不能 delete，p2 才會自動清理)" << endl;
        p2->greet();
    }

    // 離開 scope 時 p2 自動 delete（p1 已經是 nullptr，delete nullptr 是安全的）
    cout << "  即將離開 scope..." << endl;
    cout << endl;

    {
        auto new_p = make_unique<Resource>("B");
        new_p->greet();

        cout << "  即將離開 scope..." << endl;
    }
}

// ============================================================
// 1.5 為什麼串 linked list 需要 std::move？
// ============================================================
/**
 * 實例：在 linked list 最前面插入節點
 *
 *   struct Node {
 *       int value;
 *       unique_ptr<Node> next;
 *   };
 *
 *   unique_ptr<Node> head_;  // list 的開頭
 *
 *   void push_front(int value) {
 *       auto new_node = make_unique<Node>(value);
 *
 *       // 步驟 1: 把舊的 head 接到新節點後面
 *       new_node->next = std::move(head_);
 *       //              ^^^^^^^^^^^^^^^^^
 *       //   head_ 原本是 unique_ptr，裡面有「舊鏈結」的所有權。
 *       //   我們希望把這個所有權「轉移」給 new_node->next，
 *       //   所以用 std::move 告訴編譯器：把 head_ 掏空，內容給 next。
 *       //
 *       //   如果寫 new_node->next = head_; → 編譯錯誤（unique_ptr 不能複製）
 *
 *       // 步驟 2: 更新 head 指向新節點
 *       head_ = std::move(new_node);
 *       //      ^^^^^^^^^^^^^^^^^^^
 *       //   同理，new_node 的所有權轉給 head_，new_node 變成 nullptr。
 *   }
 *
 *  視覺化：
 *
 *   初始狀態:
 *     head_ → [A] → [B] → nullptr
 *     new_node → [X]
 *
 *   步驟 1 後（new_node->next = std::move(head_)）:
 *     head_ → nullptr
 *     new_node → [X] → [A] → [B] → nullptr
 *
 *   步驟 2 後（head_ = std::move(new_node)）:
 *     head_ → [X] → [A] → [B] → nullptr
 *     new_node → nullptr
 *
 * ---- 那什麼時候用 .get()？ ----
 *
 * 遍歷 list 時！遍歷只是「看一看」，不是要接手所有權，所以用 .get()：
 *
 *   string to_string() const {
 *       string result;
 *       Node* curr = head_.get();   // 借用 raw pointer 當 cursor
 *       while (curr != nullptr) {
 *           result += to_string(curr->value) + " -> ";
 *           curr = curr->next.get(); // 繼續借用下一個
 *       }
 *       return result + "null";
 *   }
 *
 * 為什麼遍歷不能用 std::move？
 *   因為 std::move(head_) 會把 head_ 掏空！遍歷完 list 就被搬光了。
 *   .get() 只是借來看，list 結構完全不動。
 *
 * 簡單記：
 *   「要換主人」→ std::move（原本的會被掏空）
 *   「只是看看」→ .get()   （原本的不受影響）
 */

// ============================================================
// 2. unique_ptr 與函式
// ============================================================

// 工廠模式：回傳 unique_ptr 表示「呼叫者擁有這個物件」
unique_ptr<Resource> create_resource(const string &name)
{
    return make_unique<Resource>(name);
}

// 借用：用 raw pointer 或 reference，不轉移 ownership
void use_resource(const Resource &res)
{
    res.greet();
}

void unique_ptr_with_functions()
{
    // Output:
    // === unique_ptr 與函式 ===
    //   [Resource 建構] Factory-Made
    //   Hello from Factory-Made
    //   即將離開 scope...
    //
    //   [Resource 解構] Factory-Made
    cout << "=== unique_ptr 與函式 ===" << endl;

    auto r = create_resource("Factory-Made");
    use_resource(*r); // 傳 reference，不轉移所有權

    cout << "  即將離開 scope..." << endl;
    cout << endl;
}

// ============================================================
// 3. shared_ptr — 共享所有權
// ============================================================
void shared_ptr_demo()
{
    // Output:
    // === shared_ptr ===
    //   [Resource 建構] Shared-B
    //   ref count: 1
    //   p2 複製後 ref count: 2
    //   p3 複製後 ref count: 3
    //   p2, p3 離開後 ref count: 1
    //   即將離開 scope...
    //
    //   [Resource 解構] Shared-B
    cout << "=== shared_ptr ===" << endl;

    auto p1 = make_shared<Resource>("Shared-B");
    cout << "  ref count: " << p1.use_count() << endl;

    {
        auto p2 = p1; // 複製 → ref count +1
        cout << "  p2 複製後 ref count: " << p1.use_count() << endl;

        auto p3 = p1; // 再複製
        cout << "  p3 複製後 ref count: " << p1.use_count() << endl;
    }
    // p2, p3 離開 scope → ref count 減回 1
    cout << "  p2, p3 離開後 ref count: " << p1.use_count() << endl;

    // p1 離開 scope 後 ref count 歸零，Resource 被 delete
    cout << "  即將離開 scope..." << endl;
    cout << endl;
}

// ============================================================
// 4. weak_ptr — 打破循環引用
// ============================================================
/**
 * 場景：雙向 linked list
 *
 * 每個 Node 需要知道「下一個」和「前一個」。
 * 如果雙向都用 shared_ptr，會形成循環引用：
 *
 *     A ──shared──→ B
 *     A ←──shared── B       ref count 永遠 >= 1，永遠不會 delete
 *
 * 解法：沿著「所有權方向」用 shared_ptr，反向用 weak_ptr：
 *
 *     head (shared) → A ─shared─→ B ─shared─→ C
 *                       ←─weak──   ←─weak──
 *
 *  - 正向 shared：head 活著 → 整條鏈都活著
 *  - 反向 weak：只是回頭看，不影響生死
 *  - head 死亡時：A 的 ref count 歸零 → A 死 → B 死 → C 死（連鎖釋放）
 */
struct Node
{
    string name;
    shared_ptr<Node> next; // 強引用：next 節點的 owner
    weak_ptr<Node> prev;   // 弱引用：只是「記得」前一個節點

    Node(const string &n) : name(n)
    {
        cout << "  [Node 建構] " << name << endl;
    }
    ~Node()
    {
        cout << "  [Node 解構] " << name << endl;
    }
};

void weak_ptr_demo()
{
    // Output:
    // === weak_ptr：雙向 linked list ===
    //   [Node 建構] A
    //   [Node 建構] B
    //   [Node 建構] C
    //   A ref count: 1  (只有 main)
    //   B ref count: 2  (main + A->next)
    //   C ref count: 2  (main + B->next)
    //   正向遍歷: A B C 
    //   反向遍歷: C B A 
    //
    //   放掉 a, b, c，觀察解構順序:
    //
    //   如果 prev 也用 shared_ptr（錯誤示範）:
    //
    //     struct BadNode {
    //         shared_ptr<BadNode> next;
    //         shared_ptr<BadNode> prev;  // 雙向都是 strong ref!
    //     };
    //     auto x = make_shared<BadNode>();
    //     auto y = make_shared<BadNode>();
    //     x->next = y;  // y.ref_count = 2
    //     y->prev = x;  // x.ref_count = 2
    //
    //     // main 結束後：
    //     //   x.ref_count = 2 - 1 = 1 (y->prev 還指著)
    //     //   y.ref_count = 2 - 1 = 1 (x->next 還指著)
    //     //   → 互相卡死，永遠不會歸零 → memory leak!
    //
    //   [Node 解構] A
    //   [Node 解構] B
    //   [Node 解構] C
    cout << "=== weak_ptr：雙向 linked list ===" << endl;

    // 建立 A <-> B <-> C 的雙向鏈結
    auto a = make_shared<Node>("A");
    auto b = make_shared<Node>("B");
    auto c = make_shared<Node>("C");

    a->next = b;
    b->prev = a; // A ─shared→ B,  A ←weak── B
    b->next = c;
    c->prev = b; // B ─shared→ C,  B ←weak── C

    // 觀察 ref count：weak_ptr 不會增加計數
    cout << "  A ref count: " << a.use_count() << "  (只有 main)" << endl;
    cout << "  B ref count: " << b.use_count() << "  (main + A->next)" << endl;
    cout << "  C ref count: " << c.use_count() << "  (main + B->next)" << endl;

    // 正向遍歷（透過 shared_ptr，直接使用）
    cout << "  正向遍歷: ";
    for (auto n = a; n; n = n->next)
    {
        cout << n->name << " ";
    }
    cout << endl;

    // 反向遍歷（透過 weak_ptr，必須先 lock()）
    cout << "  反向遍歷: ";
    for (auto n = c; n;)
    {
        cout << n->name << " ";
        n = n->prev.lock(); // weak_ptr 用 lock() 轉成 shared_ptr
    }
    cout << endl;

    // 示範：如果 head 被釋放，整條鏈都會被釋放
    cout << "\n  放掉 a, b, c，觀察解構順序:" << endl;
    // 這裡只會印出 a 的 ref count 歸零 → delete A → delete B → delete C
    cout << endl;

    // ---- 錯誤示範：兩邊都用 shared_ptr 會 leak ----
    cout << R"(  如果 prev 也用 shared_ptr（錯誤示範）:

    struct BadNode {
        shared_ptr<BadNode> next;
        shared_ptr<BadNode> prev;  // 雙向都是 strong ref!
    };
    auto x = make_shared<BadNode>();
    auto y = make_shared<BadNode>();
    x->next = y;  // y.ref_count = 2
    y->prev = x;  // x.ref_count = 2

    // main 結束後：
    //   x.ref_count = 2 - 1 = 1 (y->prev 還指著)
    //   y.ref_count = 2 - 1 = 1 (x->next 還指著)
    //   → 互相卡死，永遠不會歸零 → memory leak!
)" << endl;
}

// ============================================================
// 5. 實用模式：unique_ptr 管理陣列
// ============================================================
void unique_ptr_array()
{
    // Output:
    // === unique_ptr 管理陣列 ===
    //   陣列: 10 20 30 40 50 
    //   (實務上建議用 std::vector 取代動態陣列)
    //
    cout << "=== unique_ptr 管理陣列 ===" << endl;

    // unique_ptr 也可以管理動態陣列
    auto arr = make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i)
    {
        arr[i] = (i + 1) * 10;
    }

    cout << "  陣列: ";
    for (int i = 0; i < 5; ++i)
    {
        cout << arr[i] << " ";
    }
    cout << endl;

    // 但實務上直接用 vector 更好
    cout << "  (實務上建議用 std::vector 取代動態陣列)" << endl;
    cout << endl;
}

// ============================================================
// 6. 選擇指南
// ============================================================
void choosing_guide()
{
    // Output:
    // === Smart Pointer 選擇指南 ===
    //   unique_ptr → 預設選擇，獨佔所有權，零額外開銷
    //   shared_ptr → 真的需要多個 owner 時才用，有 ref count 開銷
    //   weak_ptr   → 搭配 shared_ptr，觀察但不持有
    //   raw ptr    → 只借用，不管理生命週期
    //
    //   原則：能用 unique_ptr 就不用 shared_ptr
    //         能用 smart pointer 就不用 raw new/delete
    cout << "=== Smart Pointer 選擇指南 ===" << endl;
    cout << "  unique_ptr → 預設選擇，獨佔所有權，零額外開銷" << endl;
    cout << "  shared_ptr → 真的需要多個 owner 時才用，有 ref count 開銷" << endl;
    cout << "  weak_ptr   → 搭配 shared_ptr，觀察但不持有" << endl;
    cout << "  raw ptr    → 只借用，不管理生命週期" << endl;
    cout << endl;
    cout << "  原則：能用 unique_ptr 就不用 shared_ptr" << endl;
    cout << "        能用 smart pointer 就不用 raw new/delete" << endl;
    cout << endl;
}

int main()
{
    unique_ptr_demo();
    unique_ptr_with_functions();
    shared_ptr_demo();
    weak_ptr_demo();
    unique_ptr_array();
    choosing_guide();

    return 0;
}
