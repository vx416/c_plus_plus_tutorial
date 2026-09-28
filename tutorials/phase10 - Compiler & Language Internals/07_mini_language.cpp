/**
 * Phase 10-7: Mini Language
 *
 * 一個極小語言：let name = integer; print name + integer;
 */

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
using namespace std;

class MiniLang {
public:
    void run(const string& source) {
        istringstream in(source);
        string word;
        while (in >> word) {
            if (word == "let") {
                string name;
                char eq;
                int value;
                char semi;
                in >> name >> eq >> value >> semi;
                variables_[name] = value;
            } else if (word == "print") {
                string name;
                char plus;
                int value;
                char semi;
                in >> name >> plus >> value >> semi;
                cout << "  print = " << variables_[name] + value << endl;
            }
        }
    }

private:
    unordered_map<string, int> variables_;
};

int main() {
    // Output:
    // === mini language ===
    //   print = 15
    cout << "=== mini language ===" << endl;
    MiniLang lang;
    lang.run("let x = 10 ; print x + 5 ;");
    cout << endl;
    return 0;
}
