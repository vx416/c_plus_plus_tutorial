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

## Phase 2 — OOP 核心

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

## Phase 7 — 建置與工具鏈

| # | 主題 | 重點 |
|---|------|------|
| 7-1 | Compilation Model | preprocessor, compilation unit, linking |
| 7-2 | Header / Source 拆分 | include guard, forward declaration, ODR |
| 7-3 | CMake | `CMakeLists.txt`, target, library linking |
| 7-4 | Sanitizers & Debugging | ASan, UBSan, Valgrind, gdb/lldb 基本操作 |
| 7-5 | Unit Testing | Google Test / Catch2 基礎 |

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
| 9-1 | File I/O | `std::fstream`, binary read/write, `mmap` |
| 9-2 | POSIX System Calls | `open`/`read`/`write`/`close`, file descriptor, `errno` |
| 9-3 | Process Management | `fork`, `exec`, `wait`, pipe, signal handling |
| 9-4 | Socket Programming | TCP/UDP socket, `bind`/`listen`/`accept`/`connect` |
| 9-5 | 實戰：HTTP Server | 用 raw socket 寫一個簡易 HTTP server |
| 9-6 | 實戰：Chat Room | 多執行緒 + socket，結合 Phase 6 concurrency |

## Phase 10 — Compiler & Language Internals

| # | 主題 | 重點 |
|---|------|------|
| 10-1 | 編譯流程總覽 | preprocessing → compilation → assembly → linking 各階段產物 |
| 10-2 | Binary Format | ELF / Mach-O 結構, `readelf`, `objdump`, section / symbol table 解讀 |
| 10-3 | Lexer | tokenization 原理, 手寫一個簡單 lexer |
| 10-4 | Parser & AST | recursive descent parser, 建構 AST |
| 10-5 | Semantic Analysis | type checking, symbol table |
| 10-6 | Code Generation | 輸出簡易 bytecode 或 x86 assembly |
| 10-7 | 實戰：Mini Language | 整合 10-3 ~ 10-6，實作一個能跑的小型語言直譯器/編譯器 |

---

## 建議學習順序

```
Phase 1 → Phase 2 → Phase 5 (move/lambda) → Phase 3 → Phase 4 → Phase 6 → Phase 7 → Phase 8 → Phase 9 → Phase 10
```

- Phase 5 的 move semantics 和 lambda 建議在 templates 之前學，因為後面的 STL 和 template 大量依賴這些概念。
- Phase 9 依賴 Phase 6 (concurrency) 的知識，建議按順序。
- Phase 10 相對獨立，可以在任何時間點開始，但建議放最後作為綜合應用。
