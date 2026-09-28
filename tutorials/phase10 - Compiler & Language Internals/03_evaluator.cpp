/**
 * Phase 10-3: Evaluator
 *
 * AST evaluator 用遞迴走訪語法樹。
 */

#include <iostream>
#include <memory>
using namespace std;

struct Expr {
    virtual ~Expr() = default;
    virtual int eval() const = 0;
};

struct NumberExpr : Expr {
    explicit NumberExpr(int value) : value(value) {}
    int value;
    int eval() const override { return value; }
};

struct BinaryExpr : Expr {
    BinaryExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    char op;
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;

    int eval() const override {
        int a = left->eval();
        int b = right->eval();
        if (op == '+') return a + b;
        if (op == '-') return a - b;
        if (op == '*') return a * b;
        return a / b;
    }
};

int main() {
    // Output:
    // === AST evaluator ===
    //   2 + 3 * 4 = 14
    cout << "=== AST evaluator ===" << endl;
    auto expr = make_unique<BinaryExpr>(
        '+',
        make_unique<NumberExpr>(2),
        make_unique<BinaryExpr>('*', make_unique<NumberExpr>(3), make_unique<NumberExpr>(4)));

    cout << "  2 + 3 * 4 = " << expr->eval() << endl << endl;
    return 0;
}
