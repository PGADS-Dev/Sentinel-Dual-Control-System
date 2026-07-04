CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Werror -I../../firmware/worker-common/include

SRC_DIR := src
COMMON_SRC_DIR := ../../firmware/worker-common/src

SRC := $(SRC_DIR)/sim_f001.c $(COMMON_SRC_DIR)/sentinel_state.c
TARGET := sim_f001

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

re: clean all

.PHONY: all run clean re
