CC := cc

SRC_DIR := src
INC_DIR := include
TESTS_DIR := tests
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

TARGET := $(BUILD_DIR)/sentinel_worker_test

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)

RM := rm -rf

SRC := $(wildcard $(SRC_DIR)/*.c)
TESTS_SRC := $(wildcard $(TESTS_DIR)/*.c)

OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
TESTS_OBJ := $(patsubst $(TESTS_DIR)/%.c,$(OBJ_DIR)/%.o,$(TESTS_SRC))

all: $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ)
	@echo "Built completed"

run: $(TARGET)
	./$(TARGET)

test: $(TARGET) $(TESTS)

$(TARGET): $(OBJ) $(TESTS_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) $(TESTS_OBJ) -o $(TARGET)
	@echo "Building $(OBJ)"
	@echo "Building $(TESTS_OBJ)"
	@echo "Built $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TESTS_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(BUILD_DIR)

re: clean all

.PHONY: all run clean re
