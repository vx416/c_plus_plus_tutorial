/**
 * 練習 4: bytecode VM
 */

#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

enum class Op { Push, Add, Mul };

struct Instruction {
    Op op;
    int operand = 0;
};

int run(const vector<Instruction>& code) {
    vector<int> stack;
    for (const Instruction& ins : code) {
        if (ins.op == Op::Push) {
            stack.push_back(ins.operand);
            continue;
        }

        int b = stack.back();
        stack.pop_back();
        int a = stack.back();
        stack.pop_back();
        stack.push_back(ins.op == Op::Add ? a + b : a * b);
    }
    return stack.back();
}

int main() {
    vector<Instruction> code{
        {Op::Push, 2},
        {Op::Push, 3},
        {Op::Push, 4},
        {Op::Mul},
        {Op::Add},
    };

    assert(run(code) == 14);

    cout << "ex04 passed!" << endl;
    return 0;
}
