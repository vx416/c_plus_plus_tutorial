# Phase 9 - System Programming

## 目錄結構

```
phase9 - System Programming/
├── 01_file_io.cpp                # std::fstream / text file I/O
├── 02_posix_file_descriptors.cpp # open / read / write / close
├── 03_process_management.cpp     # fork / waitpid / exit status
├── 04_pipe_signal.cpp            # pipe / signal handling
├── 05_socket_basics.cpp          # socketpair / read / write
├── 06_mini_http_server.cpp       # HTTP request parsing / response building
├── 07_unique_fd_and_ioctl.cpp    # RAII unique_fd / Linux driver ioctl model
├── 08_mmap_and_shared_memory.cpp # mmap / MAP_SHARED zero-copy shared memory
├── 09_io_multiplexing_poll.cpp   # poll() I/O multiplexing event loop
├── exercises/
│   ├── ex01_copy_file.cpp
│   ├── ex02_read_all_fd.cpp
│   ├── ex03_process_exit_status.cpp
│   ├── ex04_http_request_parse.cpp
│   ├── ex05_socketpair_echo.cpp
│   ├── ex06_unique_fd.cpp
│   ├── ex07_mmap_shared_buffer.cpp
│   └── ex08_poll_event_loop.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase9 - System Programming"

make
make run TARGET=01_file_io
make run-examples
make run-exercises
make clean
```

## 核心觀念

System programming 的重點是理解 OS 提供的抽象：

```text
file stream      C++ 標準庫的檔案 API，型別安全、跨平台性較好
file descriptor  POSIX 的整數 handle，socket/pipe/file/device 都能操作
unique_fd        用 move-only RAII 包裝 fd，防止提早 return 造成 fd leak
ioctl            User-space HAL 與 Kernel Driver (V4L2/LWIS/I2C) 交換控制結構的系統呼叫
mmap             將檔案或共享記憶體 (DMA-BUF/ashmem) 映射到虛擬位址空間，達成零拷貝 (Zero-Copy)
poll / epoll     I/O 多工事件迴圈，同時監聽多個 fd 而不會單點阻塞 (Android Looper 基礎)
process          fork 建立子行程，waitpid 回收子行程狀態
pipe             單機行程間通訊，一端寫入、一端讀出
signal           OS 對行程送出的非同步通知，handler 裡只能做很少的事
socket           網路或本機端點，HTTP server 最後也是讀寫 socket
```

務實規則：優先用標準庫處理一般檔案；需要 process、pipe、socket、device driver (`ioctl`)、零拷貝共享記憶體 (`mmap`) 或事件迴圈 (`poll`) 時才進 POSIX API，且一律用 RAII (`UniqueFd` / `ScopedMmap`) 管理資源。

## POSIX 呼叫心法

大多數 POSIX syscall 失敗時回傳 `-1`（`mmap` 則是 `MAP_FAILED`），並設定 `errno`：

```cpp
int fd = open(path, O_RDONLY);
if (fd == -1) {
    // errno 描述失敗原因
}
```

`read()` / `write()` 也不保證一次處理完所有 bytes。真實程式要用 loop 處理 partial read/write。

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 File I/O | 複製文字檔內容 | 簡單 |
| ex02 | 02 FD | 用 file descriptor 讀完整檔案 | 中等 |
| ex03 | 03 Process | fork child 並讀 exit status | 中等 |
| ex04 | 06 HTTP | 解析 request line | 簡單 |
| ex05 | 05 Socket | 用 socketpair 做 echo | 中等 |
| ex06 | 07 UniqueFd | 實作 move-only RAII `UniqueFd` | 中等 |
| ex07 | 08 mmap | 用 `mmap` `MAP_SHARED` 跨行程零拷貝共享陣列 | 中等 |
| ex08 | 09 poll | 用 `poll()` 監聽多個 pipe 讀取就緒事件 | 中等 |
