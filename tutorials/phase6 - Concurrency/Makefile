CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pthread

EXAMPLES = 01_thread_jthread 02_mutex_lock 03_condition_variable \
           04_async_future 05_atomic_memory_order 06_coroutines

EXERCISES = ex01_parallel_sum ex02_thread_safe_counter ex03_blocking_queue \
            ex04_thread_pool_tasks ex05_atomic_counter ex06_coroutine_generator

BUILD_DIR = build

.PHONY: all examples exercises clean help run run-examples run-exercises

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

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "用法:"
	@echo "  make"
	@echo "  make run TARGET=01_thread_jthread"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make clean"
