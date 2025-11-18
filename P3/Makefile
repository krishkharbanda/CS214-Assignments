CC = gcc
CFLAGS = -Wall -Werror -std=c99 -D_POSIX_C_SOURCE=200809L -Iinclude
LDFLAGS =
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/parse.c $(SRC_DIR)/exec.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = mysh

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/mysh.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(EXECUTABLE)

test: $(EXECUTABLE)
	@echo "Basic Tests"
	@./$(EXECUTABLE) $(TEST_DIR)/test_basic.sh
	@echo ""
	@echo "Pipe Tests"
	@./$(EXECUTABLE) $(TEST_DIR)/test_pipes.sh

.PHONY: all clean test

help:
	@echo "Makefile targets:"
	@echo "  make all     - Build mysh"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make test    - Run scripts in tests/"
	@echo "  make help    - Show this help message"
