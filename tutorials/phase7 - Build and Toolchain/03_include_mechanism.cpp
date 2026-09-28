/**
 * Phase 7-3: Include 機制
 *
 * #include 本質上是把指定檔案的內容貼進目前 translation unit。
 * 常見做法是 include header，讓 compiler 看得到 declaration；
 * 真正的 implementation 通常由 .cpp 編成 object file，再交給 linker 接起來。
 *
 * 目錄:
 *   1. header vs implementation
 *   2. <> vs ""
 *   3. include guard
 *   4. forward declaration
 *   5. include what you use
 *   6. include path
 */

#include <iostream>
#include <string>
using namespace std;

// ============================================================
// 1. header vs implementation
// ============================================================
// 本章重點：
//   include 常見是 include header，例如 <iostream> 或 "math.h"。
//   header 讓 compiler 看 declaration；library/object file 提供 implementation。
//   #include 不限制副檔名，所以技術上能 include .cpp，但普通實作被多個 .cpp
//   include 後，通常會在 link 時造成 duplicated symbol。
void header_vs_implementation_demo() {
    // Output:
    // === header vs implementation ===
    //   #include usually brings declarations from headers
    //   implementation is linked from object files or libraries
    //   including .cpp is possible but usually causes duplicate definitions
    //
    cout << "=== header vs implementation ===" << endl;
    cout << "  #include usually brings declarations from headers" << endl;
    cout << "  implementation is linked from object files or libraries" << endl;
    cout << "  including .cpp is possible but usually causes duplicate definitions" << endl << endl;
}

// ============================================================
// 2. <> vs ""
// ============================================================
// 本章重點：
//   <iostream> 是 standard library header，不是直接 link 標準庫 binary。
//   它讓 compiler 看得懂 std::cout、std::cin、operator<< 等宣告和型別。
//
//   include path 是 compiler 搜尋 header 的資料夾清單。
//   standard library 的 include path 是 compiler/toolchain 預先設定好的。
//   project/third-party header 通常要用 -I 或 CMake target_include_directories 加進去。
//
//   可以把 -I 指定的資料夾想成 include 的搜尋 root：
//     g++ -Isrc -Iinclude src/main.cpp
//
//   #include <path/header.hpp>
//     不先找目前檔案所在目錄，而是從 compiler 設定的 include roots 搜尋。
//     這些 roots 包含 -I 指定的路徑，以及 toolchain 預設的 system paths。
//     <> 不只用於 standard library，也能用於 project 或 third-party headers。
//
//   #include "path/header.hpp"
//     path/header.hpp 是相對名稱，通常先以目前 source/header 所在目錄搜尋；
//     找不到時，再到 -I 與其他 compiler include paths 搜尋。
//
//   假設專案長這樣：
//     project/
//       src/main.cpp
//       src/local.hpp
//       include/app/config.hpp
//
//   src/main.cpp 裡：
//     #include "local.hpp"
//       先找 src/local.hpp。
//
//     #include "app/config.hpp"
//       先把它當成相對路徑，找 src/app/config.hpp；找不到才去 include paths。
//       compile command 有 -Iinclude，所以接著會找到 include/app/config.hpp。
//
//     #include <app/config.hpp>
//       不優先找 src/，直接從 include roots 搜尋。
//       compile command 有 -Iinclude，所以會找到 include/app/config.hpp。
void angle_vs_quote_demo() {
    // Output:
    // === <> vs "" ===
    //   <iostream> is a standard library header
    //   <...> searches compiler include roots, including paths from -I
    //   "..." searches relative to the current file first, then include roots
    //   -Iinclude makes ./include an include root
    //
    cout << "=== <> vs \"\" ===" << endl;
    cout << "  <iostream> is a standard library header" << endl;
    cout << "  <...> searches compiler include roots, including paths from -I" << endl;
    cout << "  \"...\" searches relative to the current file first, then include roots" << endl;
    cout << "  -Iinclude makes ./include an include root" << endl;
    cout << "  angle brackets are not limited to standard library headers" << endl << endl;
}

// ============================================================
// 3. include guard
// ============================================================
// 本章重點：
//   同一個 header 可能在同一條 include 展開鏈裡被重複遇到。
//   例如 a include b、a include c、c 又 include b，b 就會被遇到兩次。
//   include guard 或 #pragma once 防止同一個 header 在同一 translation unit 被展開多次。
void include_guard_demo() {
    // Output:
    // === include guard ===
    //   #ifndef HEADER_HPP / #define HEADER_HPP / #endif
    //   or #pragma once in most modern compilers
    //
    cout << "=== include guard ===" << endl;
    cout << "  #ifndef HEADER_HPP / #define HEADER_HPP / #endif" << endl;
    cout << "  or #pragma once in most modern compilers" << endl << endl;
}

// ============================================================
// 4. forward declaration
// ============================================================
// 本章重點：
//   如果 header 只需要 pointer/reference，可以 forward declare class，減少 include。
//   如果需要知道完整大小或呼叫 member function，就必須 include 完整定義。
class User;

void print_user_pointer(const User*) {
    cout << "  pointer/reference can use forward declaration" << endl;
}

void forward_declaration_demo() {
    // Output:
    // === forward declaration ===
    //   pointer/reference can use forward declaration
    //
    cout << "=== forward declaration ===" << endl;
    print_user_pointer(nullptr);
    cout << endl;
}

// ============================================================
// 5. include what you use
// ============================================================
// 本章重點：
//   自己用到什麼型別，就 include 對應 header。
//   不要依賴別的 header 剛好幫你 include。
void include_what_you_use_demo() {
    // Output:
    // === include what you use ===
    //   include headers for the symbols this file directly uses
    //
    cout << "=== include what you use ===" << endl;
    cout << "  include headers for the symbols this file directly uses" << endl << endl;
}

// ============================================================
// 6. include path
// ============================================================
// 本章重點：
//   -I path 會把 path 加進 header 搜尋路徑，也就是 include path。
//   CMake 裡通常用 target_include_directories，不要全域亂塞 include path。
//
//   例子：
//     g++ -Iinclude src/main.cpp -c -o main.o
//
//   這表示 compiler 解析 #include 時，除了預設 system paths，也會搜尋：
//     include/
//
//   所以 source 可以寫：
//     #include "app/config.hpp"
//     #include <app/config.hpp>
//
//   這兩種都可能找到：
//     include/app/config.hpp
//
//   注意：
//     -I 只影響 compile-time header 搜尋。
//     linker 找 .o / .a / .so 是另一件事。
void include_path_demo() {
    // Output:
    // === include path ===
    //   compiler flag: -Iinclude
    //   source include: #include "app/config.hpp"
    //   resolved header: include/app/config.hpp
    //   -I is for compile-time header lookup, not link-time symbols
    //   CMake: target_include_directories(app PRIVATE include)
    cout << "=== include path ===" << endl;
    cout << "  compiler flag: -Iinclude" << endl;
    cout << "  source include: #include \"app/config.hpp\"" << endl;
    cout << "  resolved header: include/app/config.hpp" << endl;
    cout << "  -I is for compile-time header lookup, not link-time symbols" << endl;
    cout << "  CMake: target_include_directories(app PRIVATE include)" << endl << endl;
}

int main() {
    header_vs_implementation_demo();
    angle_vs_quote_demo();
    include_guard_demo();
    forward_declaration_demo();
    include_what_you_use_demo();
    include_path_demo();
    return 0;
}
