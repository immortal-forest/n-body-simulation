TARGET_EXEC := galaxy
BUILD_DIR := build
SRC_DIR := native

CC := gcc
CFLAGS := -O3 -g -Wall -Wextra -MMD -MP -std=c11
LDFLAGS := -lm

SRCS := $(wildcard $(SRC_DIR)/*.c)

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)


# The final executable
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo "Linking..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: all run clean

all: $(BUILD_DIR)/$(TARGET_EXEC)

run: all
	@mkdir -p data
	@echo "Running simulation..."
	./$(BUILD_DIR)/$(TARGET_EXEC) > data/output.csv

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) data/*.csv

-include $(OBJS:.o=.d)
