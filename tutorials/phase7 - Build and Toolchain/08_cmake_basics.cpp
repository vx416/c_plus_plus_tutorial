/**
 * Phase 7-8: CMake 基礎
 *
 * CMake 是 build system generator。
 * 你寫 CMakeLists.txt，CMake 產生 Ninja/Makefile/Xcode 等實際 build files。
 *
 * 目錄:
 *   1. target mental model
 *   2. add_executable
 *   3. target_compile_features
 *   4. target_link_libraries
 *   5. out-of-source build
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. target mental model
// ============================================================
// 本章重點：
//   Modern CMake 以 target 為中心。
//   include path、compile options、linked libraries 都應該掛在 target 上。
void target_demo() {
    // Output:
    // === target mental model ===
    //   target = executable or library plus its requirements
    //
    cout << "=== target mental model ===" << endl;
    cout << "  target = executable or library plus its requirements" << endl << endl;
}

// ============================================================
// 2. add_executable
// ============================================================
// 本章重點：
//   add_executable(app main.cpp) 建立一個 executable target。
void executable_demo() {
    // Output:
    // === add_executable ===
    //   add_executable(app main.cpp)
    //
    cout << "=== add_executable ===" << endl;
    cout << "  add_executable(app main.cpp)" << endl << endl;
}

// ============================================================
// 3. target_compile_features
// ============================================================
// 本章重點：
//   用 target_compile_features(app PRIVATE cxx_std_20) 表達需要 C++20。
//   不要只靠全域 CMAKE_CXX_STANDARD 讓依賴關係變模糊。
void compile_features_demo() {
    // Output:
    // === target_compile_features ===
    //   target_compile_features(app PRIVATE cxx_std_20)
    //
    cout << "=== target_compile_features ===" << endl;
    cout << "  target_compile_features(app PRIVATE cxx_std_20)" << endl << endl;
}

// ============================================================
// 4. target_link_libraries
// ============================================================
// 本章重點：
//   target_link_libraries 不只 link library，也傳遞 library 的 usage requirements。
//   PUBLIC/PRIVATE/INTERFACE 描述依賴是否傳給使用者。
void link_libraries_demo() {
    // Output:
    // === target_link_libraries ===
    //   PRIVATE: only this target needs it
    //   PUBLIC: this target and users need it
    //   INTERFACE: only users need it
    //
    cout << "=== target_link_libraries ===" << endl;
    cout << "  PRIVATE: only this target needs it" << endl;
    cout << "  PUBLIC: this target and users need it" << endl;
    cout << "  INTERFACE: only users need it" << endl << endl;
}

// ============================================================
// 5. out-of-source build
// ============================================================
// 本章重點：
//   build 產物不要混在 source tree。
//   常見流程：
//     cmake -S . -B build
//     cmake --build build
void build_dir_demo() {
    // Output:
    // === out-of-source build ===
    //   cmake -S . -B build
    //   cmake --build build
    cout << "=== out-of-source build ===" << endl;
    cout << "  cmake -S . -B build" << endl;
    cout << "  cmake --build build" << endl << endl;
}

int main() {
    target_demo();
    executable_demo();
    compile_features_demo();
    link_libraries_demo();
    build_dir_demo();
    return 0;
}
