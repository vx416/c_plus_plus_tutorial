# Phase 11 - Mini Tokio-style Runtime

## 定位

這個 phase 不是要完整複製 Rust Tokio。Rust Tokio 建在 `Future::poll`、waker、reactor、executor 之上；標準 C++ 沒有同等的 async runtime 抽象。

這裡實作的是 Tokio 風格的幾個實用形狀：

```text
Runtime::spawn(task)       把工作丟進 runtime，回傳 future
packaged_task<R()>         把 callable 包成 future-producing task
worker threads             固定數量 thread 從 queue 取 task
join_all                   等多個 future 全部完成
spawn_after                timer thread 延遲後把 task 丟進 runtime
graceful shutdown          runtime 解構時停止收工並 join worker
```

核心觀念：不是 `future` 指定 thread pool，而是 `Runtime::spawn()` 把「會完成 future 的 task」排到指定 runtime。

## 目錄結構

```
phase11 - Mini Tokio-style Runtime/
├── 01_runtime_spawn.cpp             # Runtime::spawn + packaged_task
├── 02_join_all.cpp                  # 等多個 future
├── 03_timer_tasks.cpp               # spawn_after
├── 04_mini_tokio_style_runtime.cpp  # 綜合實戰
├── exercises/
│   ├── ex01_spawn_future.cpp
│   ├── ex02_parallel_map.cpp
│   ├── ex03_join_all.cpp
│   ├── ex04_spawn_after.cpp
│   └── ex05_runtime_pipeline.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase11 - Mini Tokio-style Runtime"

make
make run TARGET=04_mini_tokio_style_runtime
make run-examples
make run-exercises
make clean
```

## 跟 Tokio 的差異

```text
Rust Tokio:
  async fn 產生 Future 狀態機
  executor poll future
  reactor 收 I/O readiness
  waker 喚醒 pending future

這個 C++ phase:
  spawn 接收一般 callable
  packaged_task 把 return/exception 接到 future
  worker thread 直接執行 task
  timer 用 sleep thread 示範，不是 OS reactor
```

這個版本比較像「task runtime / thread pool」，不是完整 async I/O runtime。

## 練習題說明

| 練習 | 題目 | 難度 |
|------|------|------|
| ex01 | `spawn()` 回傳 future | 簡單 |
| ex02 | 用 runtime 做 parallel map | 中等 |
| ex03 | 實作 join_all | 簡單 |
| ex04 | 實作 spawn_after | 中等 |
| ex05 | 用 runtime 串 pipeline | 中等 |
