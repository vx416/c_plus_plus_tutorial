/**
 * 練習 5: Factory Pipeline
 *
 * 用 factory 建立文字處理器，再串成 pipeline。
 *
 * 重點練習：
 *   - Factory 集中建立具體型別
 *   - Pipeline 只依賴 Processor interface
 */

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
using namespace std;

class Processor {
public:
    virtual ~Processor() = default;
    virtual string process(string input) const = 0;
};

class AddPrefix : public Processor {
public:
    string process(string input) const override {
        return "prefix:" + input;
    }
};

class AddSuffix : public Processor {
public:
    string process(string input) const override {
        return input + ":suffix";
    }
};

unique_ptr<Processor> make_processor(string_view name) {
    if (name == "prefix") {
        return make_unique<AddPrefix>();
    }
    if (name == "suffix") {
        return make_unique<AddSuffix>();
    }
    return nullptr;
}

class Pipeline {
public:
    void add(unique_ptr<Processor> processor) {
        processors_.push_back(std::move(processor));
    }

    string run(string input) const {
        for (const auto& processor : processors_) {
            input = processor->process(std::move(input));
        }
        return input;
    }

private:
    vector<unique_ptr<Processor>> processors_;
};

int main() {
    Pipeline pipeline;
    pipeline.add(make_processor("prefix"));
    pipeline.add(make_processor("suffix"));

    assert(pipeline.run("data") == "prefix:data:suffix");
    cout << "ex05 passed!" << endl;
    return 0;
}
