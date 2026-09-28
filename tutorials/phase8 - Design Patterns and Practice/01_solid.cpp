/**
 * Phase 8-1: SOLID 原則
 *
 * SOLID 是設計檢查清單，不是硬性規則。
 * 它幫你觀察 class 是否責任太多、相依太硬、擴充太痛。
 *
 * 目錄:
 *   1. Single Responsibility
 *   2. Open/Closed
 *   3. Liskov Substitution
 *   4. Interface Segregation
 *   5. Dependency Inversion
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. Single Responsibility
// ============================================================
// 本章重點：
//   一個 class 應該只有一個主要改變理由。
//   如果 User 同時處理資料、格式化、寫檔、網路，很快會變難改。
struct User {
    string name;
    int age;
};

string format_user(const User& user) {
    return user.name + "(" + to_string(user.age) + ")";
}

void srp_demo() {
    // Output:
    // === Single Responsibility ===
    //   Alice(30)
    //
    cout << "=== Single Responsibility ===" << endl;
    cout << "  " << format_user({"Alice", 30}) << endl << endl;
}

// ============================================================
// 2. Open/Closed
// ============================================================
// 本章重點：
//   Open for extension, closed for modification。
//   新增行為時，盡量新增新型別，而不是一直修改既有 if/else。
class Discount {
public:
    virtual ~Discount() = default;
    virtual int apply(int cents) const = 0;
};

class NoDiscount : public Discount {
public:
    int apply(int cents) const override { return cents; }
};

class TenPercentOff : public Discount {
public:
    int apply(int cents) const override { return cents * 90 / 100; }
};

void ocp_demo() {
    // Output:
    // === Open/Closed ===
    //   price after discount = 900
    //
    cout << "=== Open/Closed ===" << endl;
    unique_ptr<Discount> discount = make_unique<TenPercentOff>();
    cout << "  price after discount = " << discount->apply(1000) << endl << endl;
}

// ============================================================
// 3. Liskov Substitution
// ============================================================
// 本章重點：
//   Derived 應該能替代 Base，而且不破壞 Base 承諾。
//   如果 Base 說 process 可以處理任何合法 input，Derived 不能偷偷縮小條件。
class Processor {
public:
    virtual ~Processor() = default;
    virtual string process(string input) const = 0;
};

class UpperProcessor : public Processor {
public:
    string process(string input) const override {
        for (char& ch : input) {
            if (ch >= 'a' && ch <= 'z') {
                ch = static_cast<char>(ch - 'a' + 'A');
            }
        }
        return input;
    }
};

void lsp_demo() {
    // Output:
    // === Liskov Substitution ===
    //   process = ABC
    //
    cout << "=== Liskov Substitution ===" << endl;
    unique_ptr<Processor> processor = make_unique<UpperProcessor>();
    cout << "  process = " << processor->process("abc") << endl << endl;
}

// ============================================================
// 4. Interface Segregation
// ============================================================
// 本章重點：
//   不要逼 class 實作它不需要的 method。
//   小 interface 比巨大 interface 更容易替換和測試。
class Reader {
public:
    virtual ~Reader() = default;
    virtual string read() = 0;
};

class StringReader : public Reader {
public:
    explicit StringReader(string value) : value_(std::move(value)) {}
    string read() override { return value_; }

private:
    string value_;
};

void isp_demo() {
    // Output:
    // === Interface Segregation ===
    //   read = small interface
    //
    cout << "=== Interface Segregation ===" << endl;
    StringReader reader("small interface");
    cout << "  read = " << reader.read() << endl << endl;
}

// ============================================================
// 5. Dependency Inversion
// ============================================================
// 本章重點：
//   高層邏輯依賴抽象，不直接依賴具體實作。
//   這讓測試和替換底層實作更容易。
class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void notify(string_view message) = 0;
};

class ConsoleNotifier : public Notifier {
public:
    void notify(string_view message) override {
        cout << "  notify: " << message << endl;
    }
};

class SignupService {
public:
    explicit SignupService(Notifier& notifier) : notifier_(notifier) {}

    void signup(string_view name) {
        notifier_.notify(string("welcome ") + string(name));
    }

private:
    Notifier& notifier_;
};

void dip_demo() {
    // Output:
    // === Dependency Inversion ===
    //   notify: welcome Bob
    cout << "=== Dependency Inversion ===" << endl;
    ConsoleNotifier notifier;
    SignupService service(notifier);
    service.signup("Bob");
    cout << endl;
}

int main() {
    srp_demo();
    ocp_demo();
    lsp_demo();
    isp_demo();
    dip_demo();
    return 0;
}
