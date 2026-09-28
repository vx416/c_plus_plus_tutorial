/**
 * Phase 8-5: 綜合練習 Mini Pipeline
 *
 * 這個小專案把 factory、strategy、type-erased callback 串在一起。
 *
 * 目錄:
 *   1. Processor interface
 *   2. concrete processors
 *   3. factory
 *   4. pipeline
 *   5. callback hook
 */

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. Processor interface
// ============================================================
// 本章重點：
//   interface 定義 pipeline 需要的最小能力。
//   這裡只需要 process(string) -> string。
class Processor {
public:
    virtual ~Processor() = default;
    virtual string process(string input) const = 0;
};

// ============================================================
// 2. concrete processors
// ============================================================
// 本章重點：
//   concrete processor 各自處理一種變換。
//   新增 processor 不需要修改 Pipeline 本身。
class TrimProcessor : public Processor {
public:
    string process(string input) const override {
        while (!input.empty() && input.front() == ' ') {
            input.erase(input.begin());
        }
        while (!input.empty() && input.back() == ' ') {
            input.pop_back();
        }
        return input;
    }
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

// ============================================================
// 3. factory
// ============================================================
// 本章重點：
//   factory 集中建立邏輯。
//   呼叫端只說要 "trim" 或 "upper"，不直接知道具體 class 名稱。
unique_ptr<Processor> make_processor(string_view name) {
    if (name == "trim") {
        return make_unique<TrimProcessor>();
    }
    if (name == "upper") {
        return make_unique<UpperProcessor>();
    }
    return nullptr;
}

// ============================================================
// 4. pipeline
// ============================================================
// 本章重點：
//   Pipeline 管理一串 processor。
//   每一步輸出變下一步輸入。
class Pipeline {
public:
    using Hook = function<void(string_view step, string_view value)>;

    void add(unique_ptr<Processor> processor) {
        processors_.push_back(std::move(processor));
    }

    void set_hook(Hook hook) {
        hook_ = std::move(hook);
    }

    string run(string input) const {
        for (size_t i = 0; i < processors_.size(); ++i) {
            input = processors_[i]->process(std::move(input));
            if (hook_) {
                hook_("step " + to_string(i), input);
            }
        }
        return input;
    }

private:
    vector<unique_ptr<Processor>> processors_;
    Hook hook_;
};

// ============================================================
// 5. callback hook
// ============================================================
// 本章重點：
//   callback hook 用 std::function 做 type erasure。
//   Pipeline 不需要知道 hook 是 lambda、function pointer 還是 functor。
void mini_project_demo() {
    // Output:
    // === mini pipeline ===
    //   step 0: hello
    //   step 1: HELLO
    //   result = HELLO
    cout << "=== mini pipeline ===" << endl;

    Pipeline pipeline;
    pipeline.add(make_processor("trim"));
    pipeline.add(make_processor("upper"));
    pipeline.set_hook([](string_view step, string_view value) {
        cout << "  " << step << ": " << value << endl;
    });

    string result = pipeline.run("  hello  ");
    cout << "  result = " << result << endl << endl;
}

int main() {
    mini_project_demo();
    return 0;
}
