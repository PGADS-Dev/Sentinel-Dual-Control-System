CC := cc

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

TARGET := $(BUILD_DIR)/sentinel_worker_test

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)

RM := rm -rf

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) -o $(TARGET)
	@echo "Built $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(BUILD_DIR)

re: clean all

.PHONY: all run clean re
