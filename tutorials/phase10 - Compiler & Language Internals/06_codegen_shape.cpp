/**
 * Phase 10-6: Code Generation Shape
 *
 * Codegen 把 AST 轉成更接近執行的形式。這裡輸出 stack VM bytecode。
 */

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

enum class Op { Push, Add, Mul };

struct Instruction {
    Op op;
    int operand = 0;
};

struct Expr {
    virtual ~Expr() = default;
    virtual void emit(vector<Instruction>& out) const = 0;
};

struct NumberExpr : Expr {
    explicit NumberExpr(int value) : value(value) {}
    int value;
    void emit(vector<Instruction>& out) const override {
        out.push_back({Op::Push, value});
    }
};

struct BinaryExpr : Expr {
    BinaryExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    char op;
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;

    void emit(vector<Instruction>& out) const override {
        left->emit(out);
        right->emit(out);
        out.push_back({op == '+' ? Op::Add : Op::Mul, 0});
    }
};

int main() {
    // Output:
    // === codegen shape ===
    //   instruction count = 5
    cout << "=== codegen shape ===" << endl;

    auto expr = make_unique<BinaryExpr>(
        '+',
        make_unique<NumberExpr>(2),
        make_unique<BinaryExpr>('*', make_unique<NumberExpr>(3), make_unique<NumberExpr>(4)));

    vector<Instruction> code;
    expr->emit(code);

    cout << "  instruction count = " << code.size() << endl << endl;
    return 0;
}
