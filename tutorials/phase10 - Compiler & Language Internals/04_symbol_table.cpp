/**
 * Phase 10-4: Symbol Table
 *
 * Scope chain 讓內層可以 shadow 外層名稱。
 */

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class SymbolTable {
public:
    SymbolTable() {
        scopes_.push_back({});
    }

    void push_scope() {
        scopes_.push_back({});
    }

    void pop_scope() {
        scopes_.pop_back();
    }

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
    // Output:
    // === symbol table ===
    //   inner x = 2
    //   outer x = 1
    cout << "=== symbol table ===" << endl;

    SymbolTable table;
    table.define("x", 1);
    table.push_scope();
    table.define("x", 2);

    cout << "  inner x = " << *table.lookup("x") << endl;
    table.pop_scope();
    cout << "  outer x = " << *table.lookup("x") << endl;
    cout << endl;
    return 0;
}
