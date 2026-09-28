/**
 * Phase 7-1: Compilation Model
 *
 * 先問一個直覺問題：為什麼不直接把整個專案一次變成 binary？
 *
 * preprocessor 的存在目的，是把一些早期、簡單、跟 C++ 型別系統無關的工作
 * 先做完：include 展開、macro 替換、條件編譯。這讓 compiler 本體可以只面對
 * 展開後的單一 translation unit，而不用先管理高階 module/import 語意。
 *
 * 接著，每個 .cpp 通常可以獨立編譯成 object file。這讓大型專案不用每次
 * 都全量重編，也讓 library 可以預先編好。最後 linker 再把 object files
 * 和 libraries 裡的 symbol 合併成 executable。
 *
 * 目錄:
 *   1. 為什麼要分階段
 *   2. preprocessing
 *   3. translation unit
 *   4. object file
 *   5. linking
 *   6. 常見錯誤
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. 為什麼要分階段
// ============================================================
// 本章重點：
//   「直接整包變 binary」在概念上可以想像，但實務上會失去清楚的邊界。
//   C/C++ 把簡單的文字/token 處理放到 preprocessor，讓 compiler 本體保持單純。
void phase_reason_demo() {
    // Output:
    // === why compilation has phases ===
    //   preprocessor handles early text/token work
    //   compiler sees one expanded translation unit at a time
    //   linker resolves cross-file and library symbols later
    //
    cout << "=== why compilation has phases ===" << endl;
    cout << "  preprocessor handles early text/token work" << endl;
    cout << "  compiler sees one expanded translation unit at a time" << endl;
    cout << "  linker resolves cross-file and library symbols later" << endl << endl;
}

// ============================================================
// 2. preprocessing
// ============================================================
// 本章重點：
//   preprocessor 先處理 #include、#define、#if。
//   你可以用 g++ -E file.cpp 看到展開後的結果。
void preprocessing_demo() {
    // Output:
    // === preprocessing ===
    //   #include copies header text into this file
    //   #define is expanded before C++ type checking
    //
    cout << "=== preprocessing ===" << endl;
    cout << "  #include copies header text into this file" << endl;
    cout << "  #define is expanded before C++ type checking" << endl << endl;
}

// ============================================================
// 3. translation unit
// ============================================================
// 本章重點：
//   一個 .cpp 加上它 include 展開後的所有內容，叫 translation unit。
//   compiler 是以 translation unit 為單位編譯，不會自動知道其他 .cpp 的內容。
void translation_unit_demo() {
    // Output:
    // === translation unit ===
    //   one .cpp after preprocessing becomes one translation unit
    //
    cout << "=== translation unit ===" << endl;
    cout << "  one .cpp after preprocessing becomes one translation unit" << endl << endl;
}

// ============================================================
// 4. object file
// ============================================================
// 本章重點：
//   g++ -c main.cpp -o main.o 只編譯，不 link。
//   object file 裡有機器碼和 symbol，但還不是完整程式。
void object_file_demo() {
    // Output:
    // === object file ===
    //   .o contains compiled code but may still reference external symbols
    //
    cout << "=== object file ===" << endl;
    cout << "  .o contains compiled code but may still reference external symbols" << endl << endl;
}

// ============================================================
// 5. linking
// ============================================================
// 本章重點：
//   linker 把多個 object file 和 library 合成 executable。
//   如果宣告了函式但找不到定義，會出現 undefined symbol。
void linking_demo() {
    // Output:
    // === linking ===
    //   linker resolves symbols across object files and libraries
    //
    cout << "=== linking ===" << endl;
    cout << "  linker resolves symbols across object files and libraries" << endl << endl;
}

// ============================================================
// 6. 常見錯誤
// ============================================================
// 本章重點：
//   compile error：單一 translation unit 型別/語法錯。
//   link error：編譯過了，但 symbol 合併失敗。
void error_demo() {
    // Output:
    // === common errors ===
    //   compile error: syntax/type problem
    //   link error: missing or duplicated symbol
    cout << "=== common errors ===" << endl;
    cout << "  compile error: syntax/type problem" << endl;
    cout << "  link error: missing or duplicated symbol" << endl << endl;
}

int main() {
    phase_reason_demo();
    preprocessing_demo();
    translation_unit_demo();
    object_file_demo();
    linking_demo();
    error_demo();
    return 0;
}
