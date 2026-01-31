CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

TARGET = $(BIN_DIR)/ws_interpreter

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(BUILD_DIR)/%.test.o $(filter-out $(BUILD_DIR)/main.o,$(OBJ)) | $(BIN_DIR)
	$(CC) $^ -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
