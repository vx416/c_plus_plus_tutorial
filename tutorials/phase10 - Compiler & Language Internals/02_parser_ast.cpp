/**
 * Phase 10-2: Parser & AST
 *
 * Recursive descent parser 通常讓每個 precedence level 對應一個 function。
 */

#include <cctype>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

enum class TokenKind { Number, Plus, Star, LParen, RParen, End };

struct Token {
    TokenKind kind;
    string text;
};

struct Expr {
    virtual ~Expr() = default;
    virtual void print() const = 0;
};

struct NumberExpr : Expr {
    explicit NumberExpr(int value) : value(value) {}
    int value;
    void print() const override { cout << value; }
};

struct BinaryExpr : Expr {
    BinaryExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    char op;
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;
    void print() const override {
        cout << "(";
        left->print();
        cout << " " << op << " ";
        right->print();
        cout << ")";
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
        } else {
            char ch = source[i++];
            if (ch == '+') tokens.push_back({TokenKind::Plus, "+"});
            else if (ch == '*') tokens.push_back({TokenKind::Star, "*"});
            else if (ch == '(') tokens.push_back({TokenKind::LParen, "("});
            else if (ch == ')') tokens.push_back({TokenKind::RParen, ")"});
        }
    }
    tokens.push_back({TokenKind::End, ""});
    return tokens;
}

class Parser {
public:
    explicit Parser(vector<Token> tokens) : tokens_(std::move(tokens)) {}

    unique_ptr<Expr> parse_expression() {
        return parse_add();
    }

private:
    unique_ptr<Expr> parse_add() {
        auto left = parse_mul();
        while (peek().kind == TokenKind::Plus) {
            consume();
            left = make_unique<BinaryExpr>('+', std::move(left), parse_mul());
        }
        return left;
    }

    unique_ptr<Expr> parse_mul() {
        auto left = parse_primary();
        while (peek().kind == TokenKind::Star) {
            consume();
            left = make_unique<BinaryExpr>('*', std::move(left), parse_primary());
        }
        return left;
    }

    unique_ptr<Expr> parse_primary() {
        if (peek().kind == TokenKind::Number) {
            int value = stoi(consume().text);
            return make_unique<NumberExpr>(value);
        }
        if (peek().kind == TokenKind::LParen) {
            consume();
            auto expr = parse_expression();
            if (peek().kind != TokenKind::RParen) throw runtime_error("expected ')'");
            consume();
            return expr;
        }
        throw runtime_error("expected expression");
    }

    const Token& peek() const { return tokens_[pos_]; }
    const Token& consume() { return tokens_[pos_++]; }

    vector<Token> tokens_;
    size_t pos_ = 0;
};

int main() {
    // Output:
    // === parser / AST ===
    //   AST = (2 + (3 * 4))
    cout << "=== parser / AST ===" << endl;
    Parser parser(tokenize("2 + 3 * 4"));
    auto ast = parser.parse_expression();
    cout << "  AST = ";
    ast->print();
    cout << endl << endl;
    return 0;
}
