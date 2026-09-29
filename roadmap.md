# C++ Roadmap

> 跳過基礎語法（變數宣告、for/while/if、基本 I/O），從中階概念開始。

---

## Phase 1 — 記憶體與指標

| # | 主題 | 重點 |
|---|------|------|
| 1-1 | Pointers 基礎 | `*`, `&`, pointer arithmetic, null pointer |
| 1-2 | References vs Pointers | pass by value / reference / pointer 的差異 |
| 1-3 | Dynamic Memory | `new` / `delete`, memory leak 偵測 |
| 1-4 | Ownership & RAII | 資源所有權概念, scope-based 生命週期, destructor 自動釋放 |
| 1-5 | Smart Pointers | `unique_ptr`, `shared_ptr`, `weak_ptr`, ownership transfer |
| 1-6 | Arrays & `std::array` / `std::vector` | stack vs heap array, iterator 基礎 |
| 1-7 | Memory Layout & Bit Manipulation | fixed-width integers, struct alignment/padding, `packed`, register bitfields |

## Phase 2 — OOP Core & Virtual Dispatch

| # | 主題 | 重點 |
|---|------|------|
| 2-1 | Class & Struct | access specifier, constructor/destructor |
| 2-2 | Rule of Three/Five/Zero | copy ctor, copy assignment, move ctor, move assignment |
| 2-3 | Inheritance | virtual function, pure virtual, abstract class |
| 2-4 | Polymorphism | vtable 概念, `override`, `final` |
| 2-5 | Operator Overloading | `<<`, `+`, `==`, conversion operator |
| 2-6 | RTTI & `dynamic_cast` | `typeid`, 何時該用 / 不該用 |

## Phase 3 — Templates & Generic Programming

| # | 主題 | 重點 |
|---|------|------|
| 3-1 | Function Templates | type deduction, explicit instantiation |
| 3-2 | Class Templates | template specialization (full / partial) |
| 3-3 | Variadic Templates | parameter pack, fold expressions (C++17) |
| 3-4 | SFINAE & `if constexpr` | `std::enable_if`, concept 前身 |
| 3-5 | Concepts (C++20) | `requires`, 自訂 concept |

## Phase 4 — STL 深入

| # | 主題 | 重點 |
|---|------|------|
| 4-1 | Containers 總覽 | sequence / associative / unordered 的選擇 |
| 4-2 | Iterators | iterator category, custom iterator |
| 4-3 | Algorithms | `sort`, `find`, `transform`, `accumulate`, ranges (C++20) |
| 4-4 | `std::string` & `std::string_view` | SSO, lifetime 陷阱 |
| 4-5 | `std::optional`, `std::variant`, `std::any` | sum type 與型別安全 |
| 4-6 | `std::span` & `StrongId` | 連續記憶體非擁有視圖 (`subspan`)、強型別 ID 包裝 |

## Phase 5 — Modern C++ 特性

| # | 主題 | 重點 |
|---|------|------|
| 5-1 | Move Semantics 深入 | rvalue reference, `std::move`, `std::forward` |
| 5-2 | Lambda Expressions | capture list, generic lambda, `mutable` |
| 5-3 | `constexpr` & `consteval` | compile-time computation |
| 5-4 | Structured Bindings & `auto` | C++17 解構, `decltype` |
| 5-5 | Error Handling | exception vs error code, `std::expected` (C++23) |

## Phase 6 — Concurrency

| # | 主題 | 重點 |
|---|------|------|
| 6-1 | `std::thread` & `std::jthread` | 建立 thread, join / detach |
| 6-2 | Mutex & Lock | `mutex`, `lock_guard`, `unique_lock`, deadlock 避免 |
| 6-3 | Condition Variable | producer-consumer pattern |
| 6-4 | `std::async` & `std::future` | task-based concurrency |
| 6-5 | Atomic & Memory Order | `std::atomic`, `memory_order` 基礎 |
| 6-6 | Coroutines (C++20) | `co_return`, `co_yield`, `co_await`, state machine |
| 6-7 | Clang Thread Safety Annotations | `-Wthread-safety`, `GUARDED_BY`, `LOCKS_EXCLUDED`, `*Locked()` 慣用法 |

## Phase 7 — 建置與工具鏈

| # | 主題 | 重點 |
|---|------|------|
| 7-1 | Compilation Model | preprocessor, compilation unit, linking 四階段 |
| 7-2 | Preprocessor & Macro | `#define`, function-like macro, stringification `#`, token pasting `##`, macro vs template/codegen |
| 7-3 | Include 機制 | `<>` vs `""`, include path 解析規則, `-I` flag, 系統 header 位置 |
| 7-4 | Namespace 機制 | namespace 作用, `using` 的危險, ADL, anonymous namespace |
| 7-5 | Header / Source 拆分 | include guard, `#pragma once`, forward declaration, ODR |
| 7-6 | Library 類型 | static library (`.a`), dynamic library (`.so`/`.dylib`/`.dll`), `dlopen` / `dlsym`, 連結差異 |
| 7-7 | 套件管理對比 | C++ 沒有標準 package manager，vs Go modules / Rust cargo / Python pip |
| 7-8 | CMake 基礎 | `CMakeLists.txt`, target, `find_package`, `target_link_libraries` |
| 7-9 | 第三方套件管理 | vcpkg, Conan, FetchContent, git submodule 等常見做法 |
| 7-10 | Sanitizers & Debugging | ASan, UBSan, Valgrind, gdb/lldb 基本操作 |
| 7-11 | Unit Testing | Google Test / Catch2 基礎 |
| 7-12 | Dynamic Loading | `.so`/`.dylib` 載入時機、`dlopen` / `dlsym`, plugin 架構、memory mapping |
| 7-13 | ABI 穩定性、Pimpl 與 Soong | API vs ABI、Pimpl Idiom、CMake vs Android `Android.bp` |

## Phase 8 — Design Patterns & 實戰

| # | 主題 | 重點 |
|---|------|------|
| 8-1 | SOLID 原則 | 在 C++ 中的實踐 |
| 8-2 | Common Patterns | Singleton, Factory, Observer, Strategy |
| 8-3 | Type Erasure | `std::function`, 手寫 type-erased wrapper |
| 8-4 | CRTP | 靜態多型, mixin |
| 8-5 | 綜合練習 | 實作一個小型專案（e.g. mini STL container / thread pool） |

## Phase 9 — System Programming

| # | 主題 | 重點 |
|---|------|------|
| 9-1 | File I/O | `std::fstream`, text/binary file I/O 的定位 |
| 9-2 | POSIX System Calls | `open`/`read`/`write`/`close`, file descriptor, `errno` |
| 9-3 | Process Management | `fork`, `waitpid`, exit status |
| 9-4 | Pipe & Signal | `pipe`, `sigaction`, signal handler 限制 |
| 9-5 | Socket Programming | `socketpair`, socket read/write 模型 |
| 9-6 | 實戰：HTTP Server Shape | HTTP request line parsing, response building |
| 9-7 | RAII `unique_fd` & `ioctl` | move-only FD wrapper、Linux Driver `ioctl` 控制模型 |
| 9-8 | `mmap` & Zero-Copy Shared Memory | `MAP_SHARED` 零拷貝共享記憶體、`DMA-BUF`/`Gralloc` 基礎 |
| 9-9 | I/O Multiplexing (`poll`) | 非阻塞多工事件迴圈（Android `Looper` / `epoll` 基礎） |

## Phase 10 — Compiler & Language Internals

| # | 主題 | 重點 |
|---|------|------|
| 10-1 | Lexer | tokenization 原理, 手寫一個簡單 lexer |
| 10-2 | Parser & AST | recursive descent parser, 建構 AST |
| 10-3 | Evaluator | 遞迴走訪 AST 求值 |
| 10-4 | Symbol Table | scope chain, shadowing, name lookup |
| 10-5 | Bytecode VM | stack machine, instruction execution |
| 10-6 | Code Generation | AST 輸出 bytecode 的基本形狀 |
| 10-7 | 實戰：Mini Language | 整合 lexer/parser/scope/執行概念，實作小型語言 |

## Phase 11 — Concurrent 實戰：Mini Tokio-style Runtime

| # | 主題 | 重點 |
|---|------|------|
| 11-1 | Runtime::spawn | 用 `packaged_task` 包 callable，回傳 `future` |
| 11-2 | join_all | 等待多個 `future` 全部完成並收集結果 |
| 11-3 | Timer Tasks | `spawn_after`, 延遲後把 task 排進 runtime |
| 11-4 | Mini Tokio-style Runtime | fixed worker threads, task queue, graceful shutdown |
| 11-5 | 實戰練習 | parallel map, timer, pipeline |

## Phase 12 — Android Native & HAL Idioms

| # | 主題 | 重點 |
|---|------|------|
| 12-1 | `-fno-exceptions`、`StatusOr<T>` & `Sigil` Factory | 無 Exception 環境下的錯誤傳遞與 Passkey 工廠模式 |
| 12-2 | Intrusive Smart Pointers (`RefBase` / `sp<T>`) | 侵入式引用計數、`onFirstRef()`、`unique_ptr` Custom Deleter |
| 12-3 | Binder / AIDL IPC & FD 跨行程傳遞 | `Bp*` Proxy / `Bn*` Stub 架構、`SCM_RIGHTS` 零拷貝 FD 傳遞 |
| 12-4 | HAL Async Session、`ThreadPool` & `ScopedTrace` | `EnqueueWork` 非同步處理、Camera HAL Callback、`ATRACE_CALL()` |

## Phase 13 — Perf Counters, Kernel & Compiler Debugging

| # | 主題 | 重點 |
|---|------|------|
| 13-1 | Perfetto & Ftrace (`trace_marker`) | Sync Slice (`B`/`E`)、跨執行緒 Async Slice (`S`/`F` + cookie)、Counter Track (`C`) |
| 13-2 | PMU Perf Counters & Cache (`simpleperf`) | `steady_clock`、P99 Tail Latency、IPC / Cache Miss 判讀、False Sharing (`alignas(64)`) |
| 13-3 | Kernel & Driver Boundary Debugging | MMIO `volatile`、`D`-state hang 與 `-ETIMEDOUT`、零配置 Ring Buffer Flight Recorder、`debugfs`/`lockdep` |
| 13-4 | Compiler Optimization & UB Traps | `-O0` vs `-O2` 差異、Strict Aliasing (`std::bit_cast`)、Signed Overflow UB、`DoNotOptimize`、`-fno-omit-frame-pointer` |
| 13-5 | Binary Symbols, Tombstone & HWASan | `SIGSEGV` struct offset 空指標判讀、`llvm-symbolizer`/`c++filt`、ARM64 HWASan Top-Byte Pointer Tagging |

---

## 建議學習順序

```
Phase 1 → Phase 2 → Phase 5 (move/lambda) → Phase 3 → Phase 4 → Phase 6 → Phase 7 → Phase 8 → Phase 9 → Phase 12 (Android/HAL) → Phase 13 (Perf/Kernel/Compiler Debug) → Phase 10 / Phase 11
```

- Phase 5 的 move semantics 和 lambda 建議在 templates 之前學，因為後面的 STL 和 template 大量依賴這些概念。
- Phase 9 依賴 Phase 6 (concurrency) 的知識，建議按順序。
- **Android OS / HAL 開發者路線**：完成 Phase 1~9 後可直接進入 **Phase 12** 與 **Phase 13**，串聯 `-fno-exceptions`、`Sigil` Factory、`GUARDED_BY`、`unique_fd`、`mmap`、Binder/HAL 非同步架構，以及 Perfetto、PMU Perf Counters、Kernel Driver 與 Compiler/Tombstone 除錯實戰。
- Phase 10 相對獨立，可以在任何時間點開始，但建議在熟悉建置與 STL 後再做。
- Phase 11 依賴 Phase 6 的 thread/future 和 Phase 5 的 `invoke_result_t`，適合作為 concurrency 綜合應用。
