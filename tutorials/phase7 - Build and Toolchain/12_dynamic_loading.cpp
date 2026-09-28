/**
 * Phase 7-12: Dynamic Loading
 *
 * .so / .dylib / .dll 的價值不只是「省一點 memory」。
 * 它更常是為了共享 library、獨立更新、plugin 架構、optional dependency，
 * 以及建立 binary-level API 邊界。
 *
 * 目錄:
 *   1. why dynamic linking exists
 *   2. normal dynamic link
 *   3. explicit dynamic loading
 *   4. memory mapping
 *   5. tradeoffs
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. why dynamic linking exists
// ============================================================
// 本章重點：
//   static link 可以把需要的 library code 放進 executable。
//   dynamic link 則讓 executable 記錄「我需要哪個 shared library」。
//
//   如果只有單一 app、所有功能一定都會用到、不需要 plugin，也不在意更新方式，
//   static link 可能更簡單。
//
//   dynamic link 的價值主要在：
//     - 多個 process 共用 read-only code pages
//     - library 可以獨立更新
//     - plugin 可以 runtime 才載入
//     - optional dependency 不一定要跟主程式綁死
void why_dynamic_linking_demo() {
    // Output:
    // === why dynamic linking exists ===
    //   static link: copy needed library code into the executable
    //   dynamic link: executable records a dependency on a shared library
    //   useful for sharing, independent updates, plugins, optional modules
    //
    cout << "=== why dynamic linking exists ===" << endl;
    cout << "  static link: copy needed library code into the executable" << endl;
    cout << "  dynamic link: executable records a dependency on a shared library" << endl;
    cout << "  useful for sharing, independent updates, plugins, optional modules" << endl << endl;
}

// ============================================================
// 2. normal dynamic link
// ============================================================
// 本章重點：
//   一般 dynamic link 不是只 include header 就好。
//
//   source code:
//     #include "math.h"
//
//   link command:
//     g++ main.o -L./lib -lmath -o app
//
//   runtime:
//     app 啟動時，dynamic loader 找到 libmath.so，先 map 進 process，
//     然後才進 main()。
//
//   也就是：
//     include header 只讓 compiler 看 declaration。
//     -L / -l 讓 linker 知道要依賴哪個 library。
//     loader path / rpath / LD_LIBRARY_PATH 讓 runtime 找得到 .so。
void normal_dynamic_link_demo() {
    // Output:
    // === normal dynamic link ===
    //   compile: include header for declarations
    //   link:    g++ main.o -L./lib -lmath -o app
    //   run:     loader finds and maps libmath.so before main()
    //
    cout << "=== normal dynamic link ===" << endl;
    cout << "  compile: include header for declarations" << endl;
    cout << "  link:    g++ main.o -L./lib -lmath -o app" << endl;
    cout << "  run:     loader finds and maps libmath.so before main()" << endl << endl;
}

// ============================================================
// 3. explicit dynamic loading
// ============================================================
// 本章重點：
//   plugin-style loading 不在 link time 指定每個 plugin .so。
//   主程式通常掃 plugins/ folder，執行到 dlopen() 時才載入指定 library。
//
//   Linux/macOS:
//     void* handle = dlopen("./plugins/image_png.so", RTLD_NOW);
//     auto create = reinterpret_cast<CreateFn>(dlsym(handle, "create_plugin"));
//     Plugin* plugin = create();
//     dlclose(handle);
//
//   Windows:
//     LoadLibrary / GetProcAddress / FreeLibrary
//
//   這種模式仍然需要 shared ABI/API contract。
//   實務上常用 C ABI export，例如 extern "C" create_plugin，避免 C++ name mangling。
void explicit_dynamic_loading_demo() {
    // Output:
    // === explicit dynamic loading ===
    //   app scans plugins/ at runtime
    //   dlopen(path): load that shared library now
    //   dlsym(handle, name): find exported symbol address
    //   call through function pointer
    //   use C ABI for plugin boundaries when possible
    //
    cout << "=== explicit dynamic loading ===" << endl;
    cout << "  app scans plugins/ at runtime" << endl;
    cout << "  dlopen(path): load that shared library now" << endl;
    cout << "  dlsym(handle, name): find exported symbol address" << endl;
    cout << "  call through function pointer" << endl;
    cout << "  use C ABI for plugin boundaries when possible" << endl << endl;
}

// ============================================================
// 4. memory mapping
// ============================================================
// 本章重點：
//   載入 .so 不是簡單把整個檔案複製到 heap。
//   OS dynamic loader 會把 shared library 的 segments 映射進 process virtual memory。
//
//   code segment:
//     通常 read + execute，可以被多個 process 共用 physical pages。
//
//   data segment:
//     通常 read + write，每個 process 有自己的可寫狀態。
//
//   dlclose() 會降低 loader handle 的 reference count。
//   沒有人使用時，loader 可以 unmap library，但實際行為依平台和相依性而定。
void memory_mapping_demo() {
    // Output:
    // === memory mapping ===
    //   loaded .so is mapped into process virtual memory
    //   read-only code pages can be shared by multiple processes
    //   writable data pages are process-local
    //   dlclose may allow the loader to unmap the library
    //
    cout << "=== memory mapping ===" << endl;
    cout << "  loaded .so is mapped into process virtual memory" << endl;
    cout << "  read-only code pages can be shared by multiple processes" << endl;
    cout << "  writable data pages are process-local" << endl;
    cout << "  dlclose may allow the loader to unmap the library" << endl << endl;
}

// ============================================================
// 5. tradeoffs
// ============================================================
// 本章重點：
//   dynamic linking 有彈性，但不是免費午餐。
//
//   static link:
//     deployment simple, one executable, fixed versions
//
//   dynamic link:
//     smaller main binary, shared pages, independent updates, plugin loading
//     but runtime library search path / ABI compatibility / versioning 會變複雜。
void tradeoffs_demo() {
    // Output:
    // === tradeoffs ===
    //   static link: simpler deployment, larger self-contained binary
    //   dynamic link: shared libraries, plugin flexibility, update flexibility
    //   cost: runtime search paths, ABI compatibility, version management
    //
    cout << "=== tradeoffs ===" << endl;
    cout << "  static link: simpler deployment, larger self-contained binary" << endl;
    cout << "  dynamic link: shared libraries, plugin flexibility, update flexibility" << endl;
    cout << "  cost: runtime search paths, ABI compatibility, version management" << endl << endl;
}

int main() {
    why_dynamic_linking_demo();
    normal_dynamic_link_demo();
    explicit_dynamic_loading_demo();
    memory_mapping_demo();
    tradeoffs_demo();
    return 0;
}
