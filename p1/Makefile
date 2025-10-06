# To run normal tests: git run HEADERSIZE=24
# To run death tests: git run-death HEADERSIZE=24

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

# Pin the header size visible to tests (override at invoke time)
# Example: make run HEADERSIZE=24
HEADERSIZE ?= 24
CPPFLAGS   += -DHEADERSIZE=$(HEADERSIZE)

PKGCONF   ?= pkg-config
CRIT_PKG  := $(shell $(PKGCONF) --exists criterion && echo criterion)
ifeq ($(CRIT_PKG),)
  CRIT_PKG := $(shell $(PKGCONF) --exists criterion-3 && echo criterion-3)
endif

ifeq ($(CRIT_PKG),)
  BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
  ifneq ($(BREW_PREFIX),)
    CPPFLAGS += -I$(BREW_PREFIX)/include
    LDFLAGS  += -L$(BREW_PREFIX)/lib
  endif
  LDLIBS    ?= -lcriterion -lpthread
else
  CPPFLAGS  += $(shell $(PKGCONF) --cflags $(CRIT_PKG))
  LDLIBS     = $(shell $(PKGCONF) --libs   $(CRIT_PKG))
endif

# ---- Paths ----
SRC_DIR   := src
INC_DIR   := include
TEST_DIR  := tests
UNIT_DIR  := $(TEST_DIR)/unit
STRESS_DIR:= $(TEST_DIR)/stress
NEG_DIR   := $(TEST_DIR)/negative
BIN_DIR   := $(TEST_DIR)/bin

# ---- Sources ----
ALLOC_SRC := $(SRC_DIR)/mymalloc.c
UNIT_SRC  := $(wildcard $(UNIT_DIR)/*.c)
STRESS_SRC:= $(wildcard $(STRESS_DIR)/*.c)
TEST_SRC  := $(UNIT_SRC) $(STRESS_SRC)

NEG_SRC   := $(wildcard $(NEG_DIR)/*.c)
NEG_BIN   := $(patsubst $(NEG_DIR)/%.c,$(BIN_DIR)/%,$(NEG_SRC))

# ---- Outputs ----
ALLOC_TEST_BIN       := $(TEST_DIR)/allocator_tests
ALLOC_TEST_BASE_BIN  := $(TEST_DIR)/allocator_tests_baseline

# ---- Phony ----
.PHONY: all help allocator_tests allocator_tests_baseline death_tests \
        run run-baseline run-death sanitize clean

# ---- Top-level ----
all: allocator_tests

help:
	@echo "Targets:"
	@echo "  make allocator_tests             # build Criterion suite (custom allocator)"
	@echo "  make run                         # run Criterion suite"
	@echo "  make allocator_tests_baseline    # build suite with REALMALLOC"
	@echo "  make run-baseline                # run baseline suite"
	@echo "  make death_tests                 # build death-test binaries (expect exit 2)"
	@echo "  make run-death                   # run death tests (checks exit code/fixtures)"
	@echo "  make sanitize SAN=1              # run suite with ASan/UBSan"
	@echo "  make clean                       # remove build artifacts"
	@echo ""
	@echo "Variables:"
	@echo "  HEADERSIZE=<n>                   # header size visible to tests (default $(HEADERSIZE))"
	@echo "  SAN=1                            # enable sanitizers"

# Custom allocator suite
allocator_tests: $(ALLOC_TEST_BIN)

$(ALLOC_TEST_BIN): $(TEST_SRC) $(ALLOC_SRC) | $(TEST_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Baseline suite (libc malloc/free)
allocator_tests_baseline: $(ALLOC_TEST_BASE_BIN)

$(ALLOC_TEST_BASE_BIN): $(TEST_SRC) | $(TEST_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DREALMALLOC $(LDFLAGS) $^ -o $@ $(LDLIBS)

# ---- Death Tests (standalone programs) ----
death_tests: $(NEG_BIN)

$(BIN_DIR)/%: $(NEG_DIR)/%.c $(ALLOC_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

# ---- Runners ----
run: allocator_tests
	@echo "==> Running Criterion suite (custom allocator)"; echo
	$(ALLOC_TEST_BIN)

run-baseline: allocator_tests_baseline
	@echo "==> Running Criterion suite (baseline REALMALLOC)"; echo
	$(ALLOC_TEST_BASE_BIN)

# Each death test must exit(2). If a fixture exists at tests/expected/<name>.stderr,
# its contents are compared exactly against captured stderr.
run-death: death_tests
	@set -e; \
	fail=0; \
	for t in $(NEG_BIN); do \
	  name=$$(basename $$t); \
	  echo "==> $$name"; \
	  out=$$(mktemp); err=$$(mktemp); \
	  set +e; $$t >$$out 2>$$err; status=$$?; set -e; \
	  if [ $$status -ne 2 ]; then \
	    echo "   FAIL: expected exit code 2, got $$status"; \
	    fail=1; \
	  fi; \
	  fixture="$(TEST_DIR)/expected/$${name}.stderr"; \
	  if [ -f "$$fixture" ]; then \
	    if ! diff -u "$$fixture" "$$err" >/dev/null; then \
	      echo "   FAIL: stderr mismatch for $$name"; \
	      echo "   --- expected"; sed 's/^/     /' "$$fixture"; \
	      echo "   --- got";      sed 's/^/     /' "$$err"; \
	      fail=1; \
	    fi; \
	  else \
	    echo "   note: no fixture $$fixture; skipping stderr check"; \
	  fi; \
	  rm -f "$$out" "$$err"; \
	done; \
	if [ $$fail -ne 0 ]; then \
	  echo ""; echo "Death tests FAILED"; exit 1; \
	else \
	  echo ""; echo "All death tests passed"; \
	fi

sanitize:
	@$(MAKE) run SAN=1 --no-print-directory

# ---- Dirs & Cleanup ----
$(TEST_DIR):
	@mkdir -p $(TEST_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	@rm -f $(ALLOC_TEST_BIN) $(ALLOC_TEST_BASE_BIN)
	@rm -f $(NEG_BIN)
	@find . -name "*.o" -delete
	@find . -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" -delete
