/**
 * Phase 7-6: Library 類型
 *
 * library 是可重用的編譯產物或 API 集合。
 * C++ 常見有 static library 和 dynamic/shared library。
 *
 * 目錄:
 *   1. object file
 *   2. static library
 *   3. dynamic library
 *   4. explicit dynamic loading
 *   5. header vs library
 *   6. link order
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. object file
// ============================================================
// 本章重點：
//   .o 是單一 translation unit 編譯後結果。
//   library 常常只是把多個 object files 包起來。
void object_demo() {
    // Output:
    // === object file ===
    //   g++ -c math.cpp -o math.o
    //
    cout << "=== object file ===" << endl;
    cout << "  g++ -c math.cpp -o math.o" << endl << endl;
}

// ============================================================
// 2. static library
// ============================================================
// 本章重點：
//   static library 常見副檔名 .a。
//   link 時需要的機器碼會被放進 executable。
void static_library_demo() {
    // Output:
    // === static library ===
    //   ar rcs libmath.a math.o
    //   g++ main.o libmath.a -o app
    //
    cout << "=== static library ===" << endl;
    cout << "  ar rcs libmath.a math.o" << endl;
    cout << "  g++ main.o libmath.a -o app" << endl << endl;
}

// ============================================================
// 3. dynamic library
// ============================================================
// 本章重點：
//   dynamic library 在 runtime 載入，macOS 常見 .dylib，Linux 常見 .so。
//   executable 通常不包含整份 library code，而是記錄需要載入哪個 shared library。
//
//   一般動態連結：
//     app link 到 libmath.so 後，程式啟動時 dynamic loader 會先把 libmath.so
//     map 進 process address space，然後 main() 才開始跑。
//
//   手動動態載入：
//     dlopen("libplugin.so") 這類 API 才是「執行到那行時載入」。
//
//   載入後：
//     library code/data 會被映射到 process 的 virtual memory。
//     OS 可以讓多個 process 共用同一份 read-only code pages。
void dynamic_library_demo() {
    // Output:
    // === dynamic library ===
    //   Linux:  libmath.so
    //   macOS:  libmath.dylib
    //   Windows: math.dll
    //   normal dynamic link: loader maps the library before main()
    //   explicit plugin load: dlopen() loads a library when that code runs
    //   loaded library is mapped into the process virtual memory
    //
    cout << "=== dynamic library ===" << endl;
    cout << "  Linux:  libmath.so" << endl;
    cout << "  macOS:  libmath.dylib" << endl;
    cout << "  Windows: math.dll" << endl;
    cout << "  normal dynamic link: loader maps the library before main()" << endl;
    cout << "  explicit plugin load: dlopen() loads a library when that code runs" << endl;
    cout << "  loaded library is mapped into the process virtual memory" << endl << endl;
}

// ============================================================
// 4. explicit dynamic loading
// ============================================================
// 本章重點：
//   一般 dynamic link 是啟動程式時由 loader 載入 shared library。
//   explicit dynamic loading 是程式自己呼叫 dlopen / dlsym。
//
//   Linux/macOS 概念：
//     dlopen("./libplugin.so", RTLD_NOW)  載入 library，回傳 handle
//     dlsym(handle, "create_plugin")      找 symbol，回傳 address
//     dlclose(handle)                     放掉 handle
//
//   Windows 對應概念：
//     LoadLibrary / GetProcAddress / FreeLibrary
//
//   plugin 系統常用這個模式，因為 executable 不需要在 link time 就知道所有 plugin。
void explicit_dynamic_loading_demo() {
    // Output:
    // === explicit dynamic loading ===
    //   dlopen: load shared library at this line of code
    //   dlsym: find a function/data symbol by name
    //   function pointer: call code from the loaded library
    //   dlclose: release the dynamic loader handle
    //   plugin use case: load optional modules without relinking the app
    //
    cout << "=== explicit dynamic loading ===" << endl;
    cout << "  dlopen: load shared library at this line of code" << endl;
    cout << "  dlsym: find a function/data symbol by name" << endl;
    cout << "  function pointer: call code from the loaded library" << endl;
    cout << "  dlclose: release the dynamic loader handle" << endl;
    cout << "  plugin use case: load optional modules without relinking the app" << endl << endl;
}

// ============================================================
// 5. header vs library
// ============================================================
// 本章重點：
//   header 讓 compiler 知道 API 宣告。
//   library 讓 linker 找到 API 實作。
//   只有 include header 但沒有 link library，常見結果是 undefined symbol。
void header_library_demo() {
    // Output:
    // === header vs library ===
    //   header: declarations for compiler
    //   library: definitions for linker
    //
    cout << "=== header vs library ===" << endl;
    cout << "  header: declarations for compiler" << endl;
    cout << "  library: definitions for linker" << endl << endl;
}

// ============================================================
// 6. link order
// ============================================================
// 本章重點：
//   某些 linker 對 static library 順序敏感。
//   一般概念是：需要 symbol 的 object 要出現在提供 symbol 的 library 前面。
void link_order_demo() {
    // Output:
    // === link order ===
    //   if linking fails, inspect object/library order and missing symbols
    cout << "=== link order ===" << endl;
    cout << "  if linking fails, inspect object/library order and missing symbols" << endl << endl;
}

int main() {
    object_demo();
    static_library_demo();
    dynamic_library_demo();
    explicit_dynamic_loading_demo();
    header_library_demo();
    link_order_demo();
    return 0;
}
