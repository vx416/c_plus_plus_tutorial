/**
 * Phase 7-9: 第三方套件管理
 *
 * 第三方套件管理重點是可重現：別人 clone 後要知道怎麼取得同樣版本的依賴。
 *
 * 目錄:
 *   1. version pinning
 *   2. system packages
 *   3. vcpkg / Conan lock
 *   4. git submodule
 *   5. FetchContent
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. version pinning
// ============================================================
// 本章重點：
//   不要只寫「需要 fmt」。
//   要記錄版本、來源、build option，否則不同機器可能拿到不同結果。
void version_demo() {
    // Output:
    // === version pinning ===
    //   pin dependency versions for reproducible builds
    //
    cout << "=== version pinning ===" << endl;
    cout << "  pin dependency versions for reproducible builds" << endl << endl;
}

// ============================================================
// 2. system packages
// ============================================================
// 本章重點：
//   brew/apt/yum 很方便，但版本取決於系統環境。
//   適合開發工具；作為專案依賴時要清楚記錄版本。
void system_package_demo() {
    // Output:
    // === system packages ===
    //   convenient, but environment-dependent
    //
    cout << "=== system packages ===" << endl;
    cout << "  convenient, but environment-dependent" << endl << endl;
}

// ============================================================
// 3. vcpkg / Conan lock
// ============================================================
// 本章重點：
//   lockfile/profile 能幫助團隊拿到一致 dependency resolution。
void lock_demo() {
    // Output:
    // === lock/profile ===
    //   use lockfiles/profiles when reproducibility matters
    //
    cout << "=== lock/profile ===" << endl;
    cout << "  use lockfiles/profiles when reproducibility matters" << endl << endl;
}

// ============================================================
// 4. git submodule
// ============================================================
// 本章重點：
//   submodule pin 到特定 commit。
//   優點是簡單直接；缺點是更新和 nested dependency 管理比較麻煩。
void submodule_demo() {
    // Output:
    // === git submodule ===
    //   pins dependency source to a commit
    //
    cout << "=== git submodule ===" << endl;
    cout << "  pins dependency source to a commit" << endl << endl;
}

// ============================================================
// 5. FetchContent
// ============================================================
// 本章重點：
//   FetchContent 讓 CMake 在 configure 階段下載 source。
//   一定要 pin tag/commit，不要追 floating branch。
void fetch_content_demo() {
    // Output:
    // === FetchContent ===
    //   pin GIT_TAG to a release tag or commit
    cout << "=== FetchContent ===" << endl;
    cout << "  pin GIT_TAG to a release tag or commit" << endl << endl;
}

int main() {
    version_demo();
    system_package_demo();
    lock_demo();
    submodule_demo();
    fetch_content_demo();
    return 0;
}
