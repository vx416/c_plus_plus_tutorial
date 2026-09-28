/**
 * Phase 10-5: Bytecode VM
 *
 * Stack VM 用 push/pop 執行指令。
 */

#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

enum class Op { Push, Add, Mul };

struct Instruction {
    Op op;
    int operand = 0;
};

int run_vm(const vector<Instruction>& code) {
    vector<int> stack;

    for (const Instruction& ins : code) {
        if (ins.op == Op::Push) {
            stack.push_back(ins.operand);
        } else {
            if (stack.size() < 2) throw runtime_error("stack underflow");
            int b = stack.back();
            stack.pop_back();
            int a = stack.back();
            stack.pop_back();
            stack.push_back(ins.op == Op::Add ? a + b : a * b);
        }
    }

    if (stack.size() != 1) throw runtime_error("bad stack state");
    return stack.back();
}

int main() {
    // Output:
    // === bytecode VM ===
    //   result = 14
    cout << "=== bytecode VM ===" << endl;

    vector<Instruction> code{
        {Op::Push, 2},
        {Op::Push, 3},
        {Op::Push, 4},
        {Op::Mul},
        {Op::Add},
    };

    cout << "  result = " << run_vm(code) << endl << endl;
    return 0;
}
