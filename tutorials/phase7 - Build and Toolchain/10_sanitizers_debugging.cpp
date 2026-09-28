/**
 * Phase 7-10: Sanitizers & Debugging
 *
 * Sanitizer 是 runtime 檢查工具，能抓 memory bug、undefined behavior 等問題。
 *
 * 目錄:
 *   1. AddressSanitizer
 *   2. UndefinedBehaviorSanitizer
 *   3. debug symbols
 *   4. debugger mental model
 *   5. 實務流程
 */

#include <iostream>
#include <vector>
using namespace std;

// ============================================================
// 1. AddressSanitizer
// ============================================================
// 本章重點：
//   ASan 可抓 use-after-free、out-of-bounds、double-free 等。
//   編譯方式：-fsanitize=address -fno-omit-frame-pointer。
void asan_demo() {
    // Output:
    // === AddressSanitizer ===
    //   catches many memory access bugs at runtime
    //
    cout << "=== AddressSanitizer ===" << endl;
    cout << "  catches many memory access bugs at runtime" << endl << endl;
}

// ============================================================
// 2. UndefinedBehaviorSanitizer
// ============================================================
// 本章重點：
//   UBSan 可抓 signed overflow、invalid cast、除以零等 undefined behavior。
void ubsan_demo() {
    // Output:
    // === UndefinedBehaviorSanitizer ===
    //   catches undefined behavior checks supported by compiler
    //
    cout << "=== UndefinedBehaviorSanitizer ===" << endl;
    cout << "  catches undefined behavior checks supported by compiler" << endl << endl;
}

// ============================================================
// 3. debug symbols
// ============================================================
// 本章重點：
//   -g 產生 debug symbols，debugger 才能對應 source line、變數、stack frame。
void debug_symbol_demo() {
    // Output:
    // === debug symbols ===
    //   compile with -g for debugger-friendly binaries
    //
    cout << "=== debug symbols ===" << endl;
    cout << "  compile with -g for debugger-friendly binaries" << endl << endl;
}

// ============================================================
// 4. debugger mental model
// ============================================================
// 本章重點：
//   debugger 常用動作：
//     breakpoint: 停在某行
//     step: 進入下一步
//     backtrace: 看 call stack
//     print: 看變數
void debugger_demo() {
    // Output:
    // === debugger mental model ===
    //   breakpoint / step / backtrace / print
    //
    cout << "=== debugger mental model ===" << endl;
    cout << "  breakpoint / step / backtrace / print" << endl << endl;
}

// ============================================================
// 5. 實務流程
// ============================================================
// 本章重點：
//   開發時用 warning + sanitizer + tests。
//   發現 crash 時先拿 stack trace，再縮小 reproducer。
int safe_at(const vector<int>& values, size_t index) {
    if (index >= values.size()) {
        return -1;
    }
    return values[index];
}

void workflow_demo() {
    // Output:
    // === workflow ===
    //   safe_at(values, 9) = -1
    cout << "=== workflow ===" << endl;
    vector<int> values{1, 2, 3};
    cout << "  safe_at(values, 9) = " << safe_at(values, 9) << endl << endl;
}

int main() {
    asan_demo();
    ubsan_demo();
    debug_symbol_demo();
    debugger_demo();
    workflow_demo();
    return 0;
}
