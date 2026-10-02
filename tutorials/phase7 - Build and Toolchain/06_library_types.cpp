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
 *   7. 實戰陷阱 1：把同一個 .a 連結進多個 .so（Singleton / Global 狀態分裂）
 *   8. 實戰陷阱 2：.a 的自動註冊被 Linker 丟棄（--whole-archive / whole_static_libs）
 *   9. -fPIC 與 Symbol Visibility (-fvisibility=hidden)
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

// ============================================================
// 7. 實戰陷阱 1：把同一個 .a 連結進多個 .so（Singleton 分裂）
// ============================================================
// 本章重點：
//   假設 `liblogger.a` 裡有一個 `static Logger instance;`（或全域計數器 / Registry）：
//
//              ┌── static link ──> libsensor.so ──┐
//   liblogger.a│                                  ├──> camera_hal_service
//              └── static link ──> libisp.so    ──┘
//
//   因為 `.a` 只是把 `.o` 複製進目標，所以 `libsensor.so` 與 `libisp.so` 會各自擁有一份
//   獨立的 `Logger::instance()`！
//   在 runtime 看起來是同一個 class，卻有兩個不同記憶體位址的 Singleton（一邊註冊、另一邊查不到）。
//
//   務實規則：
//     - 如果一個 library 有 global/static state 且會被多個 `.so` 共用，
//       必須把它編成 shared library (`liblogger.so` / Android `shared_libs`)，不能用 `.a` (`static_libs`)！
void diamond_static_in_shared_demo() {
    // Output:
    // === Trap 1: Static .a Linked into Multiple .so (Duplicate Singletons) ===
    //   libcommon.a -> libsensor.so (gets copy #1 of static state)
    //   libcommon.a -> libisp.so    (gets copy #2 of static state)
    //   fix: stateful library shared across multiple .so must be a shared_lib (.so)
    //
    cout << "=== Trap 1: Static .a Linked into Multiple .so (Duplicate Singletons) ===" << endl;
    cout << "  libcommon.a -> libsensor.so (gets copy #1 of static state)" << endl;
    cout << "  libcommon.a -> libisp.so    (gets copy #2 of static state)" << endl;
    cout << "  fix: stateful library shared across multiple .so must be a shared_lib (.so)" << endl << endl;
}

// ============================================================
// 8. 實戰陷阱 2：.a 的自動註冊被 Linker 丟棄 (whole_static_libs)
// ============================================================
// 本章重點：
//   Linker 處理 `.a` (archive) 的預設規則是「有缺 symbol 才去搬對應的 `.o` 進來」。
//   在 Android HAL / Driver 中，常利用 global static 變數的建構子做自動註冊：
//
//     // imx989_driver.cpp (編在 libsensors.a 裡)
//     static bool registered = SensorRegistry::Register("IMX989", &CreateImx989);
//
//   如果 `main.cpp` 只呼叫 `SensorRegistry::Find("IMX989")`，完全沒直接提及 `imx989_driver.o`
//   裡的任何 symbol，Linker 就會把整個 `imx989_driver.o` 丟掉，`registered` 根本不會執行！
//
//   解法：
//     - GCC / Clang Linker: `-Wl,--whole-archive libsensors.a -Wl,--no-whole-archive`
//     - Android Soong (`Android.bp`): 把 `libsensors` 放在 `whole_static_libs: ["libsensors"]`
void whole_archive_registration_demo() {
    // Output:
    // === Trap 2: Dead-Stripped Static Registration in .a (whole_static_libs) ===
    //   default linker rule for .a: only pull .o files that resolve an undefined symbol
    //   self-registering global constructors in unreferenced .o get dropped!
    //   fix (Linux ld):   -Wl,--whole-archive libdriver.a -Wl,--no-whole-archive
    //   fix (Android.bp): whole_static_libs: ["libdriver"]
    //
    cout << "=== Trap 2: Dead-Stripped Static Registration in .a (whole_static_libs) ===" << endl;
    cout << "  default linker rule for .a: only pull .o files that resolve an undefined symbol" << endl;
    cout << "  self-registering global constructors in unreferenced .o get dropped!" << endl;
    cout << "  fix (Linux ld):   -Wl,--whole-archive libdriver.a -Wl,--no-whole-archive" << endl;
    cout << "  fix (Android.bp): whole_static_libs: [\"libdriver\"]" << endl << endl;
}

// ============================================================
// 9. -fPIC 與 Symbol Visibility (-fvisibility=hidden)
// ============================================================
// 本章重點：
//   1. `-fPIC` (Position-Independent Code)：
//      shared library (`.so`) 被載入到不同 process 時，映射的虛擬記憶體位址不固定。
//      因此編譯 `.so`（以及任何會被塞進 `.so` 的 `.a`）都必須開啟 `-fPIC`。
//   2. `-fvisibility=hidden`：
//      Linux/Android ELF 預設會把所有非 static 函式都匯出到 `.dynsym` 動態符號表，
//      導致 `.so` 變大、動態連結變慢，還可能跟其他 `.so` 發生同名符號覆蓋（Symbol Interposition）。
//      大型專案最佳實踐：編譯加 `-fvisibility=hidden`，只對公開 API 標註：
//        `__attribute__((visibility("default")))`
void fpic_and_visibility_demo() {
    // Output:
    // === -fPIC & Symbol Visibility ===
    //   -fPIC: required for .so (and any .a linked into a .so) so code works at any address
    //   -fvisibility=hidden: hide internal symbols by default, export only public APIs
    //
    cout << "=== -fPIC & Symbol Visibility ===" << endl;
    cout << "  -fPIC: required for .so (and any .a linked into a .so) so code works at any address" << endl;
    cout << "  -fvisibility=hidden: hide internal symbols by default, export only public APIs" << endl << endl;
}

int main() {
    object_demo();
    static_library_demo();
    dynamic_library_demo();
    explicit_dynamic_loading_demo();
    header_library_demo();
    link_order_demo();
    diamond_static_in_shared_demo();
    whole_archive_registration_demo();
    fpic_and_visibility_demo();
    return 0;
}
