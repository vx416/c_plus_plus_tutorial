/**
 * Phase 7-7: 套件管理對比
 *
 * C++ 沒有唯一官方 package manager。
 * 常見選擇是 vcpkg、Conan、FetchContent、git submodule、系統 package manager。
 *
 * 目錄:
 *   1. C++ 為什麼複雜
 *   2. vcpkg
 *   3. Conan
 *   4. FetchContent
 *   5. 選擇準則
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. C++ 為什麼複雜
// ============================================================
// 本章重點：
//   C++ dependency 牽涉 compiler、standard library、ABI、平台、build flags。
//   所以不像 Go modules 或 Cargo 那樣有單一官方流程。
void complexity_demo() {
    // Output:
    // === why package management is complex ===
    //   compiler, ABI, platform, and build flags all matter
    //
    cout << "=== why package management is complex ===" << endl;
    cout << "  compiler, ABI, platform, and build flags all matter" << endl << endl;
}

// ============================================================
// 2. vcpkg
// ============================================================
// 本章重點：
//   vcpkg 偏向安裝 binary/source packages 並和 CMake toolchain 整合。
//   適合應用程式專案快速取得常見 C++ library。
void vcpkg_demo() {
    // Output:
    // === vcpkg ===
    //   cmake -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake
    //
    cout << "=== vcpkg ===" << endl;
    cout << "  cmake -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake" << endl << endl;
}

// ============================================================
// 3. Conan
// ============================================================
// 本章重點：
//   Conan 是更完整的 C/C++ package manager。
//   它很重視 profile、binary compatibility、私有套件倉庫。
void conan_demo() {
    // Output:
    // === Conan ===
    //   useful when binary profiles and private packages matter
    //
    cout << "=== Conan ===" << endl;
    cout << "  useful when binary profiles and private packages matter" << endl << endl;
}

// ============================================================
// 4. FetchContent
// ============================================================
// 本章重點：
//   FetchContent 是 CMake 內建下載/加入 dependency 的方式。
//   適合小型或 header-only dependency；大型 dependency 可能讓 configure/build 變慢。
void fetch_content_demo() {
    // Output:
    // === FetchContent ===
    //   CMake downloads dependency during configure step
    //
    cout << "=== FetchContent ===" << endl;
    cout << "  CMake downloads dependency during configure step" << endl << endl;
}

// ============================================================
// 5. 選擇準則
// ============================================================
// 本章重點：
//   團隊已經有標準就跟團隊。
//   沒有標準時，應用專案可先看 vcpkg；需要嚴格 binary/profile 管理再看 Conan。
void guideline_demo() {
    // Output:
    // === package management guideline ===
    //   prefer the tool already used by the project
    //   document compiler, platform, and dependency versions
    cout << "=== package management guideline ===" << endl;
    cout << "  prefer the tool already used by the project" << endl;
    cout << "  document compiler, platform, and dependency versions" << endl << endl;
}

int main() {
    complexity_demo();
    vcpkg_demo();
    conan_demo();
    fetch_content_demo();
    guideline_demo();
    return 0;
}
