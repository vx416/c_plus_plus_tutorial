/**
 * 練習 5: symbol scope
 */

#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class ScopeStack {
public:
    ScopeStack() {
        scopes_.push_back({});
    }

    void push() { scopes_.push_back({}); }
    void pop() { scopes_.pop_back(); }

    void define(const string& name, int value) {
        scopes_.back()[name] = value;
    }

    optional<int> lookup(const string& name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) return found->second;
        }
        return nullopt;
    }

private:
    vector<unordered_map<string, int>> scopes_;
};

int main() {
    ScopeStack scopes;
    scopes.define("x", 1);
    scopes.push();
    scopes.define("x", 2);
    scopes.define("y", 3);

    assert(scopes.lookup("x") == 2);
    assert(scopes.lookup("y") == 3);

    scopes.pop();
    assert(scopes.lookup("x") == 1);
    assert(!scopes.lookup("y").has_value());

    cout << "ex05 passed!" << endl;
    return 0;
}
