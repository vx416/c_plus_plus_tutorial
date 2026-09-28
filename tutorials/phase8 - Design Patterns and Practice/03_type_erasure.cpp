/**
 * Phase 8-3: Type Erasure
 *
 * Type erasure 是「隱藏具體型別，只保留需要的操作」。
 * std::function 就是常見 type erasure：它能裝 lambda、function pointer、functor。
 *
 * 目錄:
 *   1. std::function
 *   2. why type erasure
 *   3. hand-written wrapper
 *   4. cost
 *   5. 選擇準則
 */

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// ============================================================
// 1. std::function
// ============================================================
// 本章重點：
//   std::function<R(Args...)> 可以裝任何可呼叫物件，只要簽名相容。
//   呼叫端不需要知道裡面是 lambda、function pointer 還是 class。
void std_function_demo() {
    // Output:
    // === std::function ===
    //   twice(3) = 6
    //
    cout << "=== std::function ===" << endl;

    function<int(int)> twice = [](int x) {
        return x * 2;
    };
    cout << "  twice(3) = " << twice(3) << endl << endl;
}

// ============================================================
// 2. why type erasure
// ============================================================
// 本章重點：
//   template 保留具體型別，效能好，但型別會傳染到使用者。
//   type erasure 把具體型別藏起來，API 穩定，但通常有間接呼叫成本。
void why_demo() {
    // Output:
    // === why type erasure ===
    //   stable API when concrete callable types differ
    //
    cout << "=== why type erasure ===" << endl;
    cout << "  stable API when concrete callable types differ" << endl << endl;
}

// ============================================================
// 3. hand-written wrapper
// ============================================================
// 本章重點：
//   手寫 type erasure 通常是：
//     abstract concept base
//     model<T> 存具體型別
//     wrapper 對外提供固定 API
class AnyPrinter {
public:
    template <typename T>
    explicit AnyPrinter(T value)
        : self_(make_unique<Model<T>>(std::move(value))) {}

    void print() const {
        self_->print();
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void print() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        explicit Model(T value) : value(std::move(value)) {}

        void print() const override {
            cout << "  " << value << endl;
        }

        T value;
    };

    unique_ptr<Concept> self_;
};

void hand_written_demo() {
    // Output:
    // === hand-written type erasure ===
    //   42
    //   hello
    //
    cout << "=== hand-written type erasure ===" << endl;
    vector<AnyPrinter> printers;
    printers.emplace_back(42);
    printers.emplace_back(string("hello"));
    for (const AnyPrinter& printer : printers) {
        printer.print();
    }
    cout << endl;
}

// ============================================================
// 4. cost
// ============================================================
// 本章重點：
//   type erasure 可能有 heap allocation、virtual call、失去 inline 機會。
//   這不代表不能用；代表 hot path 要量測。
void cost_demo() {
    // Output:
    // === type erasure cost ===
    //   usually trades performance detail for API simplicity
    //
    cout << "=== type erasure cost ===" << endl;
    cout << "  usually trades performance detail for API simplicity" << endl << endl;
}

// ============================================================
// 5. 選擇準則
// ============================================================
// 本章重點：
//   需要最高效且型別可暴露：template。
//   需要 runtime polymorphism 和穩定 API：virtual/type erasure。
//   只是 callback：先考慮 std::function。
void guideline_demo() {
    // Output:
    // === guideline ===
    //   callbacks: std::function
    //   custom erased API: hand-written wrapper
    cout << "=== guideline ===" << endl;
    cout << "  callbacks: std::function" << endl;
    cout << "  custom erased API: hand-written wrapper" << endl << endl;
}

int main() {
    std_function_demo();
    why_demo();
    hand_written_demo();
    cost_demo();
    guideline_demo();
    return 0;
}
