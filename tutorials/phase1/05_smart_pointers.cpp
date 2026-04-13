/**
 * Phase 1-5: Smart Pointers
 *
 * Smart pointer 是 RAII 的具體實現，讓你不需要手動 delete。
 *   - unique_ptr: 獨佔所有權，不能複製，只能移動
 *   - shared_ptr: 共享所有權，reference counting
 *   - weak_ptr:   觀察 shared_ptr 但不影響 ref count
 */

#include <iostream>
#include <memory>
using namespace std;

struct Resource {
    string name;

    explicit Resource(const string& n) : name(n) {
        cout << "  [Resource 建構] " << name << endl;
    }

    ~Resource() {
        cout << "  [Resource 解構] " << name << endl;
    }

    void greet() const {
        cout << "  Hello from " << name << endl;
    }
};

// ============================================================
// 1. unique_ptr — 獨佔所有權
// ============================================================
void unique_ptr_demo() {
    cout << "=== unique_ptr ===" << endl;

    // 用 make_unique 建立（C++14），比直接 new 更安全
    auto p1 = make_unique<Resource>("A");
    p1->greet();

    // unique_ptr 不能複製
    // auto p2 = p1;  // 編譯錯誤！

    // 但可以「移動」所有權
    auto p2 = std::move(p1);
    cout << "  移動後 p1 是否為空: " << (p1 == nullptr ? "是" : "否") << endl;
    p2->greet();

    // 離開 scope 時 p2 自動 delete
    cout << "  即將離開 scope..." << endl;
    cout << endl;
}

// ============================================================
// 2. unique_ptr 與函式
// ============================================================

// 工廠模式：回傳 unique_ptr 表示「呼叫者擁有這個物件」
unique_ptr<Resource> create_resource(const string& name) {
    return make_unique<Resource>(name);
}

// 借用：用 raw pointer 或 reference，不轉移 ownership
void use_resource(const Resource& res) {
    res.greet();
}

void unique_ptr_with_functions() {
    cout << "=== unique_ptr 與函式 ===" << endl;

    auto r = create_resource("Factory-Made");
    use_resource(*r);  // 傳 reference，不轉移所有權

    cout << "  即將離開 scope..." << endl;
    cout << endl;
}

// ============================================================
// 3. shared_ptr — 共享所有權
// ============================================================
void shared_ptr_demo() {
    cout << "=== shared_ptr ===" << endl;

    auto p1 = make_shared<Resource>("Shared-B");
    cout << "  ref count: " << p1.use_count() << endl;

    {
        auto p2 = p1;  // 複製 → ref count +1
        cout << "  p2 複製後 ref count: " << p1.use_count() << endl;

        auto p3 = p1;  // 再複製
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
struct Node {
    string name;
    shared_ptr<Node> next;     // 強引用
    weak_ptr<Node> prev;       // 弱引用 → 不增加 ref count

    Node(const string& n) : name(n) {
        cout << "  [Node 建構] " << name << endl;
    }
    ~Node() {
        cout << "  [Node 解構] " << name << endl;
    }
};

void weak_ptr_demo() {
    cout << "=== weak_ptr ===" << endl;

    auto a = make_shared<Node>("Node-A");
    auto b = make_shared<Node>("Node-B");

    a->next = b;    // A → B (strong)
    b->prev = a;    // B → A (weak，不會造成循環引用)

    cout << "  A ref count: " << a.use_count() << "  (只有 main 持有)" << endl;
    cout << "  B ref count: " << b.use_count() << "  (main + A->next)" << endl;

    // 使用 weak_ptr 前要先 lock() 轉成 shared_ptr
    if (auto prev = b->prev.lock()) {
        cout << "  B 的前一個節點: " << prev->name << endl;
    } else {
        cout << "  B 的前一個節點已經不存在了" << endl;
    }

    // 如果兩邊都用 shared_ptr，會造成循環引用 → memory leak!
    cout << R"(
  循環引用問題（錯誤示範）：
    struct Bad {
        shared_ptr<Bad> other;  // 雙向都是 strong ref
    };
    auto x = make_shared<Bad>();
    auto y = make_shared<Bad>();
    x->other = y;
    y->other = x;  // ref count 永遠不會歸零 → leak!
    )" << endl;
    cout << endl;
}

// ============================================================
// 5. 實用模式：unique_ptr 管理陣列
// ============================================================
void unique_ptr_array() {
    cout << "=== unique_ptr 管理陣列 ===" << endl;

    // unique_ptr 也可以管理動態陣列
    auto arr = make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) {
        arr[i] = (i + 1) * 10;
    }

    cout << "  陣列: ";
    for (int i = 0; i < 5; ++i) {
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
void choosing_guide() {
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

int main() {
    unique_ptr_demo();
    unique_ptr_with_functions();
    shared_ptr_demo();
    weak_ptr_demo();
    unique_ptr_array();
    choosing_guide();

    return 0;
}
