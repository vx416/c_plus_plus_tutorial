/**
 * 練習 3: eval AST
 */

#include <cassert>
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

struct AddExpr : Expr {
    AddExpr(unique_ptr<Expr> left, unique_ptr<Expr> right)
        : left(std::move(left)), right(std::move(right)) {}
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;
    int eval() const override { return left->eval() + right->eval(); }
};

int main() {
    AddExpr expr(make_unique<NumberExpr>(10), make_unique<NumberExpr>(32));
    assert(expr.eval() == 42);

    cout << "ex03 passed!" << endl;
    return 0;
}
