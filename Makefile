#const not const, size_t vs int

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.c)
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.test.o,$(TEST_SRC))

TEST_BIN = $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRC))

TARGET = $(BIN_DIR)/ws_interpreter

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(BUILD_DIR)/%.test.o $(filter-out $(BUILD_DIR)/main.o,$(OBJ)) | $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.test.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: test
test: $(TEST_BIN)
	@for t in $^; do \
		echo "Running $$t"; \
		$$t || exit 1; \
	done
	@echo "All tests passed"
$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
