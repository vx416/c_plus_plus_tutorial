/**
 * 練習 1: tokenize numbers
 *
 * 把整數和加號切成 token。
 */

#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum class TokenKind { Number, Plus, End };

struct Token {
    TokenKind kind;
    string text;
};

vector<Token> tokenize(string_view source) {
    vector<Token> tokens;
    for (size_t i = 0; i < source.size();) {
        if (isspace(static_cast<unsigned char>(source[i]))) {
            ++i;
        } else if (isdigit(static_cast<unsigned char>(source[i]))) {
            size_t start = i;
            while (i < source.size() && isdigit(static_cast<unsigned char>(source[i]))) ++i;
            tokens.push_back({TokenKind::Number, string(source.substr(start, i - start))});
        } else if (source[i] == '+') {
            tokens.push_back({TokenKind::Plus, "+"});
            ++i;
        } else {
            ++i;
        }
    }
    tokens.push_back({TokenKind::End, ""});
    return tokens;
}

int main() {
    auto tokens = tokenize("12 + 34");

    assert(tokens.size() == 4);
    assert(tokens[0].kind == TokenKind::Number);
    assert(tokens[0].text == "12");
    assert(tokens[1].kind == TokenKind::Plus);
    assert(tokens[2].text == "34");
    assert(tokens[3].kind == TokenKind::End);

    cout << "ex01 passed!" << endl;
    return 0;
}
