/**
 * Phase 5-3: constexpr & consteval
 *
 * constexpr 表示「這段邏輯可以在編譯期執行」。
 * consteval 表示「這段邏輯必須在編譯期執行」。
 *
 * 目錄:
 *   1. constexpr value
 *   2. constexpr function
 *   3. constexpr container-like data
 *   4. consteval function
 *   5. compile-time vs runtime
 */

#include <array>
#include <iostream>
using namespace std;

// ============================================================
// 1. constexpr value
// ============================================================
// 本章重點：
//   constexpr 變數必須能在編譯期決定值。
//   它可以拿來當 array size、template non-type parameter 等需要編譯期常數的地方。
void constexpr_value_demo() {
    // Output:
    // === constexpr value ===
    //   array size = 4
    //
    cout << "=== constexpr value ===" << endl;

    constexpr int size = 4;
    array<int, size> values{1, 2, 3, 4};
    cout << "  array size = " << values.size() << endl;
    cout << endl;
}

// ============================================================
// 2. constexpr function
// ============================================================
// 本章重點：
//   constexpr function 可以在編譯期執行，也可以在 runtime 執行。
//   是否真的編譯期執行，取決於呼叫結果用在哪裡。
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

void constexpr_function_demo() {
    // Output:
    // === constexpr function ===
    //   factorial(5) compile-time = 120
    //   factorial(6) runtime      = 720
    //
    cout << "=== constexpr function ===" << endl;

    constexpr int compile_time = factorial(5);
    int n = 6;
    int runtime = factorial(n);

    cout << "  factorial(5) compile-time = " << compile_time << endl;
    cout << "  factorial(6) runtime      = " << runtime << endl;
    cout << endl;
}

// ============================================================
// 3. constexpr container-like data
// ============================================================
// 本章重點：
//   constexpr 可以用來在編譯期建立查表資料。
//   這適合固定規則、固定大小的資料，例如平方表、狀態轉換表。
constexpr array<int, 5> make_squares() {
    array<int, 5> values{};
    for (size_t i = 0; i < values.size(); ++i) {
        values[i] = static_cast<int>(i * i);
    }
    return values;
}

void constexpr_data_demo() {
    // Output:
    // === constexpr data ===
    //   squares[4] = 16
    //
    cout << "=== constexpr data ===" << endl;

    constexpr auto squares = make_squares();
    cout << "  squares[4] = " << squares[4] << endl;
    cout << endl;
}

// ============================================================
// 4. consteval function
// ============================================================
// 本章重點：
//   consteval function 必須在編譯期被呼叫。
//   如果你把 runtime 變數傳進 consteval function，會編譯失敗。
consteval int protocol_version() {
    return 3;
}

void consteval_demo() {
    // Output:
    // === consteval ===
    //   protocol version = 3
    //
    cout << "=== consteval ===" << endl;

    constexpr int version = protocol_version();
    cout << "  protocol version = " << version << endl;
    cout << endl;
}

// ============================================================
// 5. compile-time vs runtime
// ============================================================
// 本章重點：
//   constexpr 不是「一定比較快」的魔法。
//   它適合把明確固定的計算提前，並讓型別系統使用該結果。
//   如果資料本來就是 runtime 才知道，constexpr function 仍會在 runtime 執行。
void boundary_demo() {
    // Output:
    // === compile-time vs runtime ===
    //   constexpr: may run at compile time
    //   consteval: must run at compile time
    cout << "=== compile-time vs runtime ===" << endl;
    cout << "  constexpr: may run at compile time" << endl;
    cout << "  consteval: must run at compile time" << endl;
    cout << endl;
}

int main() {
    constexpr_value_demo();
    constexpr_function_demo();
    constexpr_data_demo();
    consteval_demo();
    boundary_demo();
    return 0;
}
