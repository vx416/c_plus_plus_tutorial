CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SANITIZE_FLAGS = -fsanitize=address -g

# 範例檔案
EXAMPLES = 01_pointers 02_references_vs_pointers 03_dynamic_memory \
           04_ownership_raii 05_smart_pointers 06_arrays_and_containers \
           07_memory_layout_and_bits

# 練習題
EXERCISES = ex01_pointer_swap ex02_array_reverse ex03_find_max \
            ex04_dynamic_matrix ex05_raii_logger ex06_unique_ptr_list \
            ex07_vector_stats ex08_register_bitfields

BUILD_DIR = build

# ============================
# 主要指令
# ============================

.PHONY: all examples exercises clean help

all: examples exercises

examples: $(addprefix $(BUILD_DIR)/, $(EXAMPLES))

exercises:
	@for f in $(EXERCISES); do \
		$(MAKE) --no-print-directory $(BUILD_DIR)/$$f 2>&1 || echo "  [SKIP] $$f (編譯失敗，可能有 TODO 未完成)"; \
	done

# 編譯範例
$(BUILD_DIR)/%: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# 編譯練習題
$(BUILD_DIR)/ex%: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ============================
# 執行
# ============================

# 執行單一檔案: make run TARGET=ex01_pointer_swap
.PHONY: run
run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

# 執行所有範例
.PHONY: run-examples
run-examples: examples
	@for f in $(EXAMPLES); do \
		echo "\n========== $$f =========="; \
		./$(BUILD_DIR)/$$f; \
	done

# 執行所有練習題測試
.PHONY: run-exercises
run-exercises:
	@for f in $(EXERCISES); do \
		echo "--- $$f ---"; \
		$(MAKE) --no-print-directory $(BUILD_DIR)/$$f 2>/dev/null && \
		(./$(BUILD_DIR)/$$f && echo "  PASS" || echo "  FAIL") || \
		echo "  SKIP (編譯失敗)"; \
	done

# ============================
# 用 AddressSanitizer 編譯（偵測 memory leak）
# ============================

.PHONY: sanitize
sanitize: $(BUILD_DIR)/$(TARGET)_san
$(BUILD_DIR)/%_san: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@
$(BUILD_DIR)/ex%_san: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@

# ============================
# 清除
# ============================

clean:
	rm -rf $(BUILD_DIR)

# ============================
# 說明
# ============================

help:
	@echo "用法:"
	@echo "  make                    編譯所有範例和練習題"
	@echo "  make examples           只編譯範例"
	@echo "  make exercises          只編譯練習題"
	@echo "  make run TARGET=ex01_pointer_swap    編譯並執行指定檔案"
	@echo "  make run-examples       執行所有範例"
	@echo "  make run-exercises      執行所有練習題"
	@echo "  make sanitize TARGET=ex04_dynamic_matrix  用 ASan 編譯（偵測 memory leak）"
	@echo "  make clean              清除編譯產物"
