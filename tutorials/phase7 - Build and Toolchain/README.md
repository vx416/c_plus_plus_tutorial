# Phase 7 - Build and Toolchain

## 目錄結構

```
phase7 - Build and Toolchain/
├── 01_compilation_model.cpp       # preprocessing / compilation / assembly / linking
├── 02_preprocessor_macro.cpp      # #define / # / ## / conditional compilation
├── 03_include_mechanism.cpp       # include guard / <> vs ""
├── 04_namespace.cpp               # namespace / using / ADL / anonymous namespace
├── 05_header_source_split.cpp     # header/source split / ODR / forward declaration
├── 06_library_types.cpp           # static / dynamic library, dlopen/dlsym
├── 07_package_management.cpp      # C++ package manager landscape
├── 08_cmake_basics.cpp            # target-based CMake mental model
├── 09_third_party_management.cpp  # vcpkg / Conan / FetchContent / submodule
├── 10_sanitizers_debugging.cpp    # ASan / UBSan / debugger basics
├── 11_unit_testing.cpp            # assert / test framework shape
├── 12_dynamic_loading.cpp         # .so loading, dlopen/dlsym, plugin shape
├── 13_abi_stability_and_pimpl.cpp # ABI compatibility, Pimpl idiom, Android.bp vs CMake
├── exercises/
│   ├── ex01_include_guard.cpp
│   ├── ex02_macro_debug.cpp
│   ├── ex03_namespace_api.cpp
│   ├── ex04_static_library_shape.cpp
│   ├── ex05_assert_tests.cpp
│   └── ex06_pimpl_sensor_driver.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase7 - Build and Toolchain"

make
make run TARGET=01_compilation_model
make run-examples
make run-exercises
make sanitize TARGET=10_sanitizers_debugging
make clean
```

## 核心觀念

先問一個直覺問題：為什麼不直接把整個專案一次變成 binary？

preprocessor 的存在目的，是把一些早期、簡單、跟 C++ 型別系統無關的工作先做完：
include 展開、macro 替換、條件編譯。這讓 compiler 本體可以只面對展開後的
單一 translation unit，而不用先管理高階 module/import 語意。

接著每個 `.cpp` 通常獨立編譯成 object file。這樣大型專案只需要重編改過的
檔案，也可以把 library 預先編好。最後 linker 再把多個 object files 和
libraries 合成 executable。

C++ 工具鏈通常可以拆成四段：

```text
preprocess  展開 #include / #define / #if
compile     把每個 translation unit 編成 object file
assemble    產生機器碼 object
link        把 object/library 合成 executable
```

平常寫程式最容易踩的是：

```text
include 太多       編譯變慢、相依變複雜
macro 太大         型別系統看不到，debug 困難
header 定義錯誤    ODR/link error
link 觀念不清楚    undefined symbol / duplicate symbol
```

## Include 心法

`#include` 本質上是把指定檔案內容貼進目前 translation unit。常見做法是
include header，例如 `<iostream>` 或 `"math.h"`，讓 compiler 看得到 API 的
declaration；真正的 implementation 通常由 `.cpp` 編成 object file，再交給
linker 接起來。

如果同一個 header 在同一條 include 展開鏈裡被遇到多次，例如 `a` include `b`、
`a` include `c`、`c` 又 include `b`，就需要 include guard 或 `#pragma once`
避免同一個 translation unit 裡重複定義。

`include path` 是 compiler 搜尋 header 的資料夾清單。`#include "local.hpp"`
通常先找目前檔案所在目錄；`#include <vector>` 這類 standard library header
走 compiler/toolchain 預設的 system include paths。自己的 `include/` 目錄通常
要用 `-Iinclude` 或 CMake `target_include_directories(app PRIVATE include)`
加進搜尋路徑。

例如：

```text
project/
  src/main.cpp
  src/local.hpp
  include/app/config.hpp
```

`src/main.cpp` 裡寫 `#include "local.hpp"` 會先找到 `src/local.hpp`。
如果寫 `#include "app/config.hpp"` 或 `#include <app/config.hpp>`，通常要用：

```bash
g++ -Iinclude -c src/main.cpp -o main.o
```

讓 compiler 能找到 `include/app/config.hpp`。`-I` 只解決 compile-time header
搜尋；linker 找 implementation symbols 是另一階段。

## 常見命令

```bash
# 只做 preprocessor，觀察 include/macro 展開後結果
g++ -E file.cpp

# 只編譯成 object file，不 link
g++ -c file.cpp -o file.o

# link object files
g++ main.o lib.o -o app

# build a shared library
g++ -fPIC -shared plugin.cpp -o libplugin.so

# explicit dynamic loading API
dlopen("./libplugin.so") -> dlsym(handle, "symbol_name")

# 啟用 sanitizer
g++ -fsanitize=address,undefined -fno-omit-frame-pointer file.cpp -o app
```

## CMake 最小心法

Modern CMake 優先用 target：

```cmake
add_executable(app main.cpp)
target_compile_features(app PRIVATE cxx_std_20)
target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE mylib)
```

不要把 CMake 當全域變數腳本來寫。把 include path、compile option、library dependency 綁在 target 上。

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | Include | 寫 include guard 形狀示範 | 簡單 |
| ex02 | Macro | debug macro + function 替代 | 簡單 |
| ex03 | Namespace | 設計 namespace API | 簡單 |
| ex04 | Library | 模擬 library API 邊界 | 中等 |
| ex05 | Unit Testing | assert-based test runner | 簡單 |
| ex06 | ABI & Pimpl | 用 Pimpl Idiom 隱藏實作並維持固定 sizeof | 中等 |
