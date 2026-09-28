/**
 * Phase 10-1: Lexer
 *
 * Lexer 把 source text 切成 token。
 */

#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum class TokenKind { Number, Plus, Star, LParen, RParen, End };

struct Token {
    TokenKind kind;
    string text;
};

vector<Token> tokenize(string_view source) {
    vector<Token> tokens;

    for (size_t i = 0; i < source.size();) {
        char ch = source[i];
        if (isspace(static_cast<unsigned char>(ch))) {
            ++i;
        } else if (isdigit(static_cast<unsigned char>(ch))) {
            size_t start = i;
            while (i < source.size() && isdigit(static_cast<unsigned char>(source[i]))) {
                ++i;
            }
            tokens.push_back({TokenKind::Number, string(source.substr(start, i - start))});
        } else {
            switch (ch) {
                case '+': tokens.push_back({TokenKind::Plus, "+"}); break;
                case '*': tokens.push_back({TokenKind::Star, "*"}); break;
                case '(': tokens.push_back({TokenKind::LParen, "("}); break;
                case ')': tokens.push_back({TokenKind::RParen, ")"}); break;
                default: cout << "  skip unknown char: " << ch << endl; break;
            }
            ++i;
        }
    }

    tokens.push_back({TokenKind::End, ""});
    return tokens;
}

int main() {
    // Output:
    // === lexer ===
    //   token: 12
    //   token: +
    //   token: 3
    //   token: *
    //   token: (
    //   token: 4
    //   token: +
    //   token: 5
    //   token: )
    //   token: 
    cout << "=== lexer ===" << endl;
    for (const Token& token : tokenize("12 + 3 * (4 + 5)")) {
        cout << "  token: " << token.text << endl;
    }
    cout << endl;
    return 0;
}
