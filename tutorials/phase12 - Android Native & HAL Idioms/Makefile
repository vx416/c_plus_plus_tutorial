CXX = g++
# 模擬 Android AOSP / HAL 真實編譯環境：關閉 exceptions 與 RTTI，並啟用 Clang thread-safety 檢查
CXXFLAGS = -std=c++20 -Wall -Wextra -Wno-unknown-warning-option -Wthread-safety -fno-exceptions -fno-rtti -pthread
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -g

EXAMPLES = 01_fno_exceptions_and_sigil_factory 02_intrusive_refbase_sp_wp \
           03_binder_aidl_ipc_shape 04_hal_session_and_scoped_trace

EXERCISES = ex01_sigil_status_or_factory ex02_intrusive_sp \
            ex03_fd_passing_scm_rights ex04_scoped_atrace_and_callback

BUILD_DIR = build

.PHONY: all examples exercises clean help run run-examples run-exercises sanitize

all: examples exercises

examples: $(addprefix $(BUILD_DIR)/, $(EXAMPLES))

exercises: $(addprefix $(BUILD_DIR)/, $(EXERCISES))

$(BUILD_DIR)/%: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/ex%: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

run-examples: examples
	@for f in $(EXAMPLES); do \
		echo "\n========== $$f =========="; \
		./$(BUILD_DIR)/$$f; \
	done

run-exercises: exercises
	@for f in $(EXERCISES); do \
		echo "--- $$f ---"; \
		./$(BUILD_DIR)/$$f && echo "  PASS" || echo "  FAIL"; \
	done

sanitize: $(BUILD_DIR)/$(TARGET)_san
	./$(BUILD_DIR)/$(TARGET)_san

$(BUILD_DIR)/%_san: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@

$(BUILD_DIR)/ex%_san: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "用法:"
	@echo "  make"
	@echo "  make run TARGET=01_fno_exceptions_and_sigil_factory"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make sanitize TARGET=ex02_intrusive_sp"
	@echo "  make clean"
