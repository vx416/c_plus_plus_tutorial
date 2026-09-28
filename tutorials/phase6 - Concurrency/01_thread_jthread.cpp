/**
 * Phase 6-1: std::thread & std::jthread
 *
 * thread 讓一段函式在另一條執行線上跑。
 * jthread 是 C++20 版本，離開 scope 時會自動 join，還支援 stop_token。
 *
 * 目錄:
 *   1. 建立 thread
 *   2. join vs detach
 *   3. detach 安全示範
 *   4. 傳參數給 thread
 *   5. jthread 自動 join
 *   6. stop_token 合作式停止
 *   7. stop_source 中心化停止訊號
 */

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
using namespace std;
using namespace std::chrono_literals;

// ============================================================
// 1. 建立 thread
// ============================================================
// 本章重點：
//   std::thread 接收一個 callable，立刻開始在新 thread 執行。
//   main thread 會繼續往下跑，所以輸出順序不應依賴運氣。
void create_thread_demo() {
    // Output:
    // === create thread ===
    //   worker: running
    //   main: worker joined
    //
    cout << "=== create thread ===" << endl;

    thread worker([] {
        cout << "  worker: running" << endl;
    });
    worker.join();
    cout << "  main: worker joined" << endl << endl;
}

// ============================================================
// 2. join vs detach
// ============================================================
// 本章重點：
//   join 表示等待 thread 結束。
//   detach 表示讓 thread 自己在背景跑，呼叫端不再管理它。
//
//   一般教學和業務程式優先用 join/jthread。
//   detach 很容易造成生命週期問題：背景 thread 還在用的物件可能已經消失。
void join_detach_demo() {
    // Output:
    // === join vs detach ===
    //   join: caller waits for thread completion
    //   detach: caller gives up ownership; use carefully
    //
    cout << "=== join vs detach ===" << endl;
    cout << "  join: caller waits for thread completion" << endl;
    cout << "  detach: caller gives up ownership; use carefully" << endl;
    cout << endl;
}

// ============================================================
// 3. detach 安全示範
// ============================================================
// 本章重點：
//   detach 後，std::thread object 不再代表那條執行線。
//   你不能 join，也不能知道它何時結束。
//
//   detach 最危險的是生命週期：
//     detached thread 不能引用可能先消失的區域變數。
//
//   這裡用 shared_ptr 管理 done flag，確保背景 thread 和呼叫端都持有同一份狀態。
//   這只是示範 detach 的安全邊界；一般業務程式仍優先用 join 或 jthread。
void detach_demo() {
    // Output:
    // === detach safe boundary ===
    //   detached worker completed without touching dead locals
    //
    cout << "=== detach safe boundary ===" << endl;

    auto done = make_shared<atomic<bool>>(false);

    thread background([done] {
        this_thread::sleep_for(5ms);
        done->store(true);
    });
    background.detach();

    while (!done->load()) {
        this_thread::sleep_for(1ms);
    }

    cout << "  detached worker completed without touching dead locals" << endl;
    cout << endl;
}

// ============================================================
// 4. 傳參數給 thread
// ============================================================
// 本章重點：
//   thread 參數預設會 copy/move 進新 thread。
//   如果要傳 reference，必須用 std::ref 明確表示。
void append_suffix(string& text, const string& suffix) {
    text += suffix;
}

void argument_demo() {
    // Output:
    // === thread arguments ===
    //   text = hello thread
    //
    cout << "=== thread arguments ===" << endl;

    string text = "hello";
    thread worker(append_suffix, std::ref(text), " thread");
    worker.join();
    cout << "  text = " << text << endl << endl;
}

// ============================================================
// 5. jthread 自動 join
// ============================================================
// 本章重點：
//   std::thread 如果 destructor 時還 joinable，程式會 terminate。
//   std::jthread destructor 會自動 request_stop 並 join，較不容易忘記收尾。
void jthread_demo() {
    // Output:
    // === jthread auto join ===
    //   leaving inner scope will join automatically
    //   jthread worker: done
    //   jthread has joined
    //
    cout << "=== jthread auto join ===" << endl;

    {
        jthread worker([] {
            cout << "  jthread worker: done" << endl;
        });
        cout << "  leaving inner scope will join automatically" << endl;
    }

    cout << "  jthread has joined" << endl;
    cout << endl;
}

// ============================================================
// 6. stop_token 合作式停止
// ============================================================
// 本章重點：
//   stop_token 不是強制殺 thread。
//   它只是讓 worker 可以定期檢查「有人要求我停嗎」。
//   這叫 cooperative cancellation，worker 要自己配合停下來。
void stop_token_demo() {
    // Output:
    // === stop_token ===
    //   stop requested
    //   worker stopped after ticks = 3
    //
    cout << "=== stop_token ===" << endl;

    atomic<int> stopped_ticks{0};

    {
        jthread worker([&](stop_token stop) {
        int ticks = 0;
        while (!stop.stop_requested() && ticks < 5) {
            ++ticks;
            this_thread::sleep_for(5ms);
        }
        stopped_ticks.store(ticks);
        });

        this_thread::sleep_for(12ms);
        worker.request_stop();
        cout << "  stop requested" << endl;
    }

    cout << "  worker stopped after ticks = " << stopped_ticks.load() << endl;
    cout << endl;
}

// ============================================================
// 7. stop_source 中心化停止訊號
// ============================================================
// 本章重點：
//   stop_source 是「發出停止要求的人」。
//   stop_token 是「接收停止要求的人」。
//
//   多個 worker 可以拿同一個 stop_token。
//   source.request_stop() 一次通知所有拿到該 token 的 worker。
//
//   這很像 Go context 的 cancel signal 部分：
//     context.Context -> stop_token
//     cancel()        -> stop_source::request_stop()
//
//   但 C++ stop_token 只管停止訊號，不內建 deadline / value bag。
void stop_source_demo() {
    // Output:
    // === stop_source shared signal ===
    //   stopped workers = 2
    cout << "=== stop_source shared signal ===" << endl;

    stop_source source;
    stop_token token = source.get_token();
    atomic<int> stopped_workers{0};

    auto worker_loop = [&](stop_token shared_token) {
        while (!shared_token.stop_requested()) {
            this_thread::sleep_for(2ms);
        }
        stopped_workers.fetch_add(1);
    };

    {
        jthread a(worker_loop, token);
        jthread b(worker_loop, token);

        this_thread::sleep_for(6ms);
        source.request_stop();
    } // a 和 b 在這裡自動 join

    cout << "  stopped workers = " << stopped_workers.load() << endl;
    cout << endl;
}

int main() {
    create_thread_demo();
    join_detach_demo();
    detach_demo();
    argument_demo();
    jthread_demo();
    stop_token_demo();
    stop_source_demo();
    return 0;
}
