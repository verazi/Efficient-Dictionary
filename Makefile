# Compiler and flags
# Check if gcc-11 exists
ifneq ($(shell command -v gcc-11 2>/dev/null),)
  CXX = gcc-11
  CC = gcc-11
else
  CXX = gcc
  CC = gcc
endif

# Set DEBUG to 1 for debug build, 0 for release build
DEBUG ?= 0

ifeq ($(DEBUG), 1)
    CXXFLAGS = -std=c++11 -Iinclude -Igtest/include -Igtest -g
    CFLAGS = -Iinclude -g
else
    CXXFLAGS = -std=c++11 -Iinclude -Igtest/include -Igtest -O3
    CFLAGS = -Iinclude -O3
endif

LDFLAGS = -lpthread -lstdc++

# Add OpenMP flags
CFLAGS += -fopenmp -lm
CXXFLAGS += -fopenmp -lm
LDFLAGS += -fopenmp -lm

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
BUILD_GTEST_DIR = $(BUILD_DIR)/gtest

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

MAIN_SRC_FILE := run_sort_unique_words.c
TEST_SRC_FILE := $(TEST_DIR)/test_main.cc

# GTest
GTEST_SRCS = gtest/src/gtest-all.cc
GTEST_MAIN_SRCS = gtest/src/gtest_main.cc
GTEST_OBJS = $(BUILD_GTEST_DIR)/gtest-all.o

# Default target
# Otherwise CLion cannot compile with format in all targets
all: run test format

# GTest object files
$(GTEST_OBJS): $(GTEST_SRCS)
	@mkdir -p $(BUILD_GTEST_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $(GTEST_OBJS)

# Rule to compile each .c file to its corresponding .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


run: $(MAIN_SRC_FILE) $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $(BUILD_DIR)/$@

run_baseline: $(MAIN_SRC_FILE) $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $(BUILD_DIR)/$@ -DBASELINE

test: $(TEST_SRC_FILE) $(GTEST_OBJS) $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $(BUILD_DIR)/$@
	@touch files/test/permission-denied.txt
	@chmod 220 files/test/permission-denied.txt
	@$(BUILD_DIR)/test

CLANG_FORMAT_PATH := $(shell command -v clang-format 2>/dev/null)
format:
ifdef CLANG_FORMAT_PATH
	$(CLANG_FORMAT_PATH) -i $(SRC_FILES) $(MAIN_SRC_FILE) $(TEST_SRC_FILE)
else
	@echo "[!] clang-format not found, skipping formatting"
endif

VER ?=
archive:
	@git archive --format=tar.gz --prefix skeleton/ -o comp90025-skeleton-v$(VER).tar.gz HEAD

clean:
	rm -rf $(BUILD_DIR)/*
