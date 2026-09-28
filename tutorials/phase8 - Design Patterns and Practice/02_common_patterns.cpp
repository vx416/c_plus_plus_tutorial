/**
 * Phase 8-2: Common Patterns
 *
 * Pattern 是常見設計問題的命名解法。
 * 重點不是背名字，而是知道它解決哪種變動。
 *
 * 目錄:
 *   1. Singleton
 *   2. Factory
 *   3. Observer
 *   4. Strategy
 *   5. 何時不要用 pattern
 */

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. Singleton
// ============================================================
// 本章重點：
//   Singleton 保證只有一個 instance，常見於全域設定或 registry。
//   但它也是全域狀態，會讓測試和相依關係變硬；能注入 dependency 時別先用 Singleton。
class Config {
public:
    static Config& instance() {
        static Config config;
        return config;
    }

    string env = "dev";

private:
    Config() = default;
};

void singleton_demo() {
    // Output:
    // === Singleton ===
    //   env = dev
    //
    cout << "=== Singleton ===" << endl;
    cout << "  env = " << Config::instance().env << endl << endl;
}

// ============================================================
// 2. Factory
// ============================================================
// 本章重點：
//   Factory 把「要建立哪個具體型別」集中起來。
//   呼叫端依賴抽象 Product，不需要知道 make_unique<Concrete>() 分散在哪裡。
class Shape {
public:
    virtual ~Shape() = default;
    virtual string name() const = 0;
};

class Circle : public Shape {
public:
    string name() const override { return "circle"; }
};

class Square : public Shape {
public:
    string name() const override { return "square"; }
};

unique_ptr<Shape> make_shape(string_view type) {
    if (type == "circle") {
        return make_unique<Circle>();
    }
    if (type == "square") {
        return make_unique<Square>();
    }
    return nullptr;
}

void factory_demo() {
    // Output:
    // === Factory ===
    //   shape = circle
    //
    cout << "=== Factory ===" << endl;
    auto shape = make_shape("circle");
    cout << "  shape = " << shape->name() << endl << endl;
}

// ============================================================
// 3. Observer
// ============================================================
// 本章重點：
//   Observer 解決「事件發生後，有多個訂閱者要反應」。
//   發事件的人不需要知道每個訂閱者的具體型別。
class EventBus {
public:
    using Handler = function<void(string_view)>;

    void subscribe(Handler handler) {
        handlers_.push_back(std::move(handler));
    }

    void publish(string_view event) const {
        for (const Handler& handler : handlers_) {
            handler(event);
        }
    }

private:
    vector<Handler> handlers_;
};

void observer_demo() {
    // Output:
    // === Observer ===
    //   log event: user.created
    //
    cout << "=== Observer ===" << endl;
    EventBus bus;
    bus.subscribe([](string_view event) {
        cout << "  log event: " << event << endl;
    });
    bus.publish("user.created");
    cout << endl;
}

// ============================================================
// 4. Strategy
// ============================================================
// 本章重點：
//   Strategy 解決「演算法會變」。
//   呼叫端不寫 if/else 判斷策略，而是把策略物件傳進去。
using PriceStrategy = function<int(int)>;

int checkout(int cents, const PriceStrategy& strategy) {
    return strategy(cents);
}

void strategy_demo() {
    // Output:
    // === Strategy ===
    //   checkout = 500
    //
    cout << "=== Strategy ===" << endl;
    PriceStrategy half_off = [](int cents) {
        return cents / 2;
    };
    cout << "  checkout = " << checkout(1000, half_off) << endl << endl;
}

// ============================================================
// 5. 何時不要用 pattern
// ============================================================
// 本章重點：
//   沒有變動點時，不要先加抽象。
//   一個直接的 function 可能比 class hierarchy 更好。
void avoid_pattern_demo() {
    // Output:
    // === avoid over-engineering ===
    //   add abstractions when variation is real, not imagined
    cout << "=== avoid over-engineering ===" << endl;
    cout << "  add abstractions when variation is real, not imagined" << endl << endl;
}

int main() {
    singleton_demo();
    factory_demo();
    observer_demo();
    strategy_demo();
    avoid_pattern_demo();
    return 0;
}
