# Phase 6 - Concurrency

## 目錄結構

```
phase6 - Concurrency/
├── 01_thread_jthread.cpp          # thread / jthread / join / detach / stop_token / stop_source
├── 02_mutex_lock.cpp              # mutex / lock_guard / unique_lock
├── 03_condition_variable.cpp      # producer-consumer pattern
├── 04_async_future.cpp            # async / future / thread pool / packaged_task
├── 05_atomic_memory_order.cpp     # atomic / memory order 基礎
├── 06_coroutines.cpp              # coroutine / co_return / co_yield / co_await
├── exercises/
│   ├── ex01_parallel_sum.cpp
│   ├── ex02_thread_safe_counter.cpp
│   ├── ex03_blocking_queue.cpp
│   ├── ex04_thread_pool_tasks.cpp
│   ├── ex05_atomic_counter.cpp
│   └── ex06_coroutine_generator.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase6 - Concurrency"

make
make run TARGET=01_thread_jthread
make run-examples
make run-exercises
make clean
```

## 核心觀念

Concurrency 的重點不是「開很多 thread」，而是正確管理共享狀態。

```text
thread / jthread          建立執行單位
detach                    放棄管理 thread，必須特別注意生命週期
stop_source / stop_token  中心化取消訊號與 worker 停止檢查
mutex / lock              保護共享資料
condition_variable        讓 thread 等待某個條件成立
future / async            取得背景工作結果
thread pool               固定 worker 數量，明確控制 task queue
atomic                    對單一變數做不可分割操作
memory order              控制 atomic 之間的可見性規則
coroutine                 語言層 state machine，不等於 async runtime
```

先記一條務實規則：只要多個 thread 會讀寫同一份資料，而且至少一個會寫，就需要同步。

## Async / Coroutine 定位

`std::async` 是標準庫 task launcher，不能指定使用者提供的 thread pool，也不是 epoll/kqueue/IOCP runtime。

```text
std::async / std::future:
  future 只是之後拿結果的 handle，本身不負責執行。
  async 可以安排 callable 執行，但不保證 system thread pool。
  預設 policy 可能 async，也可能 deferred；deferred 會在 get/wait 的呼叫端執行。
  不提供 async I/O event loop。

promise / packaged_task:
  promise 讓你手動填入 future 的結果。
  packaged_task 把 callable 包成會產生 future 的 task。
  兩者都由你決定背後是哪條 thread 或哪個 pool 在執行。

自寫 ThreadPool / library thread pool:
  固定 worker 數量，submit task，回傳 future。

C++20 coroutine:
  語言提供 co_return / co_yield / co_await 來產生 state machine。
  標準庫沒有 Tokio-style executor/runtime。
```

要做大量 async network I/O，通常看 Boost.Asio、standalone Asio、libuv、folly 等 library，
而不是直接靠 `std::async`。

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Thread | 平行加總 vector | 中等 |
| ex02 | 02 Mutex | thread-safe counter | 簡單 |
| ex03 | 03 Condition Variable | blocking queue | 中等 |
| ex04 | 04 Async | thread pool 平行平方加總 | 中等 |
| ex05 | 05 Atomic | atomic counter | 簡單 |
| ex06 | 06 Coroutine | generator 產生整數序列 | 中等 |

## 建議學習順序

先看 `thread` 和 `mutex`。`condition_variable` 比較容易卡，重點是「永遠用 predicate 檢查條件」。
`atomic memory_order` 先懂 `relaxed`、`release/acquire` 的用途即可，不需要一開始就背完整記憶體模型。
