/**
 * 練習 6: Unique Ptr Linked List
 *
 * 用 unique_ptr 實作一個 singly linked list，支援：
 *   - push_front: 在頭部插入
 *   - pop_front:  移除頭部並回傳值
 *   - to_string:  回傳 "1 -> 2 -> 3 -> null" 格式的字串
 *
 * 重點：完全不使用 raw new/delete，靠 unique_ptr 自動管理記憶體。
 */

#include <iostream>
#include <memory>
#include <string>
#include <cassert>
using namespace std;

struct Node {
    int value;
    unique_ptr<Node> next;

    explicit Node(int v) : value(v), next(nullptr) {}
};

class LinkedList {
public:
    LinkedList() : head_(nullptr) {}

    void push_front(int value) {
        // TODO: 建立新節點，插入到 head 前面
        // 提示：用 make_unique 建立，用 std::move 轉移 ownership
    }

    int pop_front() {
        // TODO: 移除 head 節點，回傳它的值
        // 提示：需要把 head 的 next 移動到 head
        return 0; // placeholder
    }

    string to_string() const {
        // TODO: 回傳 "1 -> 2 -> 3 -> null" 格式
        // 提示：遍歷時用 raw pointer (head_.get()) 借用，不轉移 ownership
        return ""; // placeholder
    }

    bool empty() const { return head_ == nullptr; }

private:
    unique_ptr<Node> head_;
};

int main() {
    LinkedList list;
    assert(list.empty());
    assert(list.to_string() == "null");

    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    assert(list.to_string() == "1 -> 2 -> 3 -> null");

    assert(list.pop_front() == 1);
    assert(list.to_string() == "2 -> 3 -> null");

    assert(list.pop_front() == 2);
    assert(list.pop_front() == 3);
    assert(list.empty());

    // 壓力測試：確認沒有 memory leak
    for (int i = 0; i < 10000; ++i) {
        list.push_front(i);
    }
    // list 離開 scope 時 unique_ptr 會自動釋放所有節點

    cout << "ex06 passed!" << endl;
    cout << "（建議用 -fsanitize=address 編譯確認沒有 memory leak）" << endl;
    return 0;
}
