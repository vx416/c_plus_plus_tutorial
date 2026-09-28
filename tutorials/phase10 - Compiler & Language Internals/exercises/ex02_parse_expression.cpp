/**
 * 練習 2: parse expression
 *
 * 驗證 parser 會讓乘法優先於加法。
 */

#include <cassert>
#include <cctype>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

enum class TokenKind { Number, Plus, Star, End };

struct Token {
    TokenKind kind;
    string text;
};

struct Expr {
    virtual ~Expr() = default;
    virtual string debug() const = 0;
};

struct NumberExpr : Expr {
    explicit NumberExpr(int value) : value(value) {}
    int value;
    string debug() const override { return to_string(value); }
};

struct BinaryExpr : Expr {
    BinaryExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    char op;
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;
    string debug() const override {
        return "(" + left->debug() + string(1, op) + right->debug() + ")";
    }
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
        } else if (source[i] == '*') {
            tokens.push_back({TokenKind::Star, "*"});
            ++i;
        } else {
            ++i;
        }
    }
    tokens.push_back({TokenKind::End, ""});
    return tokens;
}

class Parser {
public:
    explicit Parser(vector<Token> tokens) : tokens_(std::move(tokens)) {}

    unique_ptr<Expr> parse_expression() {
        auto left = parse_term();
        while (peek().kind == TokenKind::Plus) {
            consume();
            left = make_unique<BinaryExpr>('+', std::move(left), parse_term());
        }
        return left;
    }

private:
    unique_ptr<Expr> parse_term() {
        auto left = parse_primary();
        while (peek().kind == TokenKind::Star) {
            consume();
            left = make_unique<BinaryExpr>('*', std::move(left), parse_primary());
        }
        return left;
    }

    unique_ptr<Expr> parse_primary() {
        int value = stoi(consume().text);
        return make_unique<NumberExpr>(value);
    }

    const Token& peek() const { return tokens_[pos_]; }
    const Token& consume() { return tokens_[pos_++]; }

    vector<Token> tokens_;
    size_t pos_ = 0;
};

int main() {
    Parser parser(tokenize("2 + 3 * 4"));
    auto ast = parser.parse_expression();
    assert(ast->debug() == "(2+(3*4))");

    cout << "ex02 passed!" << endl;
    return 0;
}
