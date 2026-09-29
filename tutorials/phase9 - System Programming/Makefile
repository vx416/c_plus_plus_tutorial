CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

EXAMPLES = 01_file_io 02_posix_file_descriptors 03_process_management \
           04_pipe_signal 05_socket_basics 06_mini_http_server \
           07_unique_fd_and_ioctl 08_mmap_and_shared_memory \
           09_io_multiplexing_poll

EXERCISES = ex01_copy_file ex02_read_all_fd ex03_process_exit_status \
            ex04_http_request_parse ex05_socketpair_echo \
            ex06_unique_fd ex07_mmap_shared_buffer ex08_poll_event_loop

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
	@echo "  make run TARGET=01_file_io"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make clean"
