# ---- Compiler & Flags ----
CC        ?= gcc
CSTD      ?= c11
WARN      ?= -Wall -Wextra -Werror
DBG       ?= -g
CFLAGS    ?= -std=$(CSTD) $(WARN) $(DBG)

# Always search our local headers first
CPPFLAGS  ?= -Iinclude

# Sanitizers: enable with `make run SAN=1`
SANFLAGS  := -fsanitize=address,undefined -fno-omit-frame-pointer
ifeq ($(SAN),1)
  CFLAGS  += $(SANFLAGS)
  LDFLAGS += $(SANFLAGS)
endif

# Header size visible to allocator
HEADERSIZE ?= 24
CPPFLAGS   += -DHEADERSIZE=$(HEADERSIZE)

# ---- Paths ----
SRC_DIR    := src
INC_DIR    := include
TEST_DIR   := tests
UNIT_DIR   := $(TEST_DIR)/unit
STRESS_DIR := $(TEST_DIR)/stress
NEG_DIR    := $(TEST_DIR)/negative
BIN_DIR    := $(TEST_DIR)/bin

# ---- Sources ----
ALLOC_SRC  := $(SRC_DIR)/mymalloc.c
UNIT_SRC   := $(wildcard $(UNIT_DIR)/*.c)
STRESS_SRC := $(wildcard $(STRESS_DIR)/*.c)
NEG_SRC    := $(wildcard $(NEG_DIR)/*.c)

# ---- Outputs ----
UNIT_BIN   := $(patsubst $(UNIT_DIR)/%.c,$(BIN_DIR)/%,$(UNIT_SRC))
STRESS_BIN := $(patsubst $(STRESS_DIR)/%.c,$(BIN_DIR)/%,$(STRESS_SRC))
NEG_BIN    := $(patsubst $(NEG_DIR)/%.c,$(BIN_DIR)/%,$(NEG_SRC))

# ---- Phony Targets ----
.PHONY: all help tests death_tests run run-death clean sanitize

# ---- Default ----
all: tests

help:
	@echo "Targets:"
	@echo "  make tests                # build all test binaries"
	@echo "  make run                  # build & run all normal tests"
	@echo "  make death_tests          # build all death-test binaries"
	@echo "  make run-death            # run all death tests (expect exit 2)"
	@echo "  make sanitize SAN=1       # enable ASan/UBSan when building"
	@echo "  make clean                # remove all build artifacts"
	@echo ""
	@echo "Variables:"
	@echo "  HEADERSIZE=<n>            # header size passed to compiler (default: 24)"

# ---- Build Rules ----
$(BIN_DIR)/%: $(UNIT_DIR)/%.c $(ALLOC_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/%: $(STRESS_DIR)/%.c $(ALLOC_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/%: $(NEG_DIR)/%.c $(ALLOC_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

# ---- Build Groups ----
tests: $(UNIT_BIN) $(STRESS_BIN)
death_tests: $(NEG_BIN)

# ---- Runners ----
run: tests
	@echo "==> Running all normal tests..."; echo
	@set -e; for t in $(UNIT_BIN) $(STRESS_BIN); do \
	  echo "Running $$t"; \
	  ./$$t || exit 1; \
	  echo ""; \
	done; \
	echo "All normal tests passed successfully."

run-death: death_tests
	@echo "==> Running death tests (expect exit code 2)..."; echo
	@set -e; fail=0; \
	for t in $(NEG_BIN); do \
	  name=$$(basename $$t); \
	  echo "Running $$name"; \
	  set +e; ./$$t > /dev/null 2>&1; status=$$?; set -e; \
	  if [ $$status -ne 2 ]; then \
	    echo "   FAIL: expected exit code 2, got $$status"; \
	    fail=1; \
	  fi; \
	done; \
	if [ $$fail -ne 0 ]; then \
	  echo ""; echo "Death tests FAILED"; exit 1; \
	else \
	  echo ""; echo "All death tests passed"; \
	fi

sanitize:
	@$(MAKE) run SAN=1 --no-print-directory

# ---- Directory Setup & Cleanup ----
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BIN_DIR)
	@find . -name "*.o" -delete
	@find . -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" -delete
	@echo "Done."
