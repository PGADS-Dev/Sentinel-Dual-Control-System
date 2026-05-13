CC := cc

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)

RM := rm -rf

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

all: $(OBJ)
	@echo "Built !"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Building..."

clean:
	$(RM) $(BUILD_DIR)

re: clean all

.PHONY: all clean re
