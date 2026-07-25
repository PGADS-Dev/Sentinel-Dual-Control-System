CC := cc

SRC_DIR := src
INC_DIR := include
TESTS_DIR := tests
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

TARGET := $(BUILD_DIR)/sentinel_worker_test
FAULT_TARGET := $(BUILD_DIR)/sentinel_fault_test

CFLAGS := -std=c11 -Wall -Wextra -Werror -Wpedantic -I$(INC_DIR)

RM := rm -rf

SRC := $(wildcard $(SRC_DIR)/*.c)
TESTS_SRC := $(TESTS_DIR)/test_sentinel_state.c
FAULT_TEST_SRC := $(TESTS_DIR)/test_sentinel_fault_validation.c

OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
TESTS_OBJ := $(OBJ_DIR)/test_sentinel_state.o
FAULT_TEST_OBJ := $(OBJ_DIR)/test_sentinel_fault_validation.o

all: $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ)
	@echo "Build completed"

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET)

fault-test: $(FAULT_TARGET)
	@echo "Running fault validation tests..."
	./$(FAULT_TARGET)

$(TARGET): $(OBJ) $(TESTS_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) $(TESTS_OBJ) -o $(TARGET)
	@echo "Built $(TARGET)"

$(FAULT_TARGET): $(OBJ) $(FAULT_TEST_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) $(FAULT_TEST_OBJ) -o $(FAULT_TARGET)
	@echo "Built $(FAULT_TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_sentinel_state.o: $(TESTS_SRC)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_sentinel_fault_validation.o: $(FAULT_TEST_SRC)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(BUILD_DIR)

re: clean all

.PHONY: all run test fault-test clean re
