CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Source directories
SRCDIR = src
TESTDIR = tests
INCDIR = include

# Source files
MYMALLOC_SRC = $(SRCDIR)/mymalloc.c
MYMALLOC_HDR = $(INCDIR)/mymalloc.h

# Main targets
all: memtest memgrind test1 test2 test3a test3b test3c test4 test5

# memtest (provided by professor)
memtest: memtest.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o memtest memtest.c $(MYMALLOC_SRC)

# memtest with leak detection
memtest-leak: memtest.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -DLEAK -o memtest-leak memtest.c $(MYMALLOC_SRC)

# memtest with real malloc (for comparison)
memtest-real: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -o memtest-real memtest.c

# memgrind (stress testing)
memgrind: memgrind.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o memgrind memgrind.c $(MYMALLOC_SRC)

# Test programs in tests directory
test1: $(TESTDIR)/test1/test1.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test1 $(TESTDIR)/test1/test1.c $(MYMALLOC_SRC)

test2: $(TESTDIR)/test2/test2.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test2 $(TESTDIR)/test2/test2.c $(MYMALLOC_SRC)

test3a: $(TESTDIR)/test3/test3a.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test3a $(TESTDIR)/test3/test3a.c $(MYMALLOC_SRC)

test3b: $(TESTDIR)/test3/test3b.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test3b $(TESTDIR)/test3/test3b.c $(MYMALLOC_SRC)

test3c: $(TESTDIR)/test3/test3c.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test3c $(TESTDIR)/test3/test3c.c $(MYMALLOC_SRC)

test4: $(TESTDIR)/test4/test4.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test4 $(TESTDIR)/test4/test4.c $(MYMALLOC_SRC)

test5: $(TESTDIR)/test5/test5.c $(MYMALLOC_SRC) $(MYMALLOC_HDR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o test5 $(TESTDIR)/test5/test5.c $(MYMALLOC_SRC)

# Testing targets
test: all
	@echo "Running all tests..."
	@echo "=== Test 1: Basic functionality ==="
	./test1
	@echo
	@echo "=== Test 2: Coalescing ==="
	./test2
	@echo
	@echo "=== Test 4: Edge cases ==="
	./test4
	@echo
	@echo "=== Test 5: Leak detection ==="
	./test5
	@echo
	@echo "=== Professor's memtest ==="
	./memtest
	@echo
	@echo "=== Professor's memtest with leaks ==="
	./memtest-leak
	@echo
	@echo "=== Stress test ==="
	./memgrind
	@echo
	@echo "Note: Tests 3a, 3b, and 3c should be run individually as they exit with error codes"

# Test error detection (these will exit with error codes)
test-errors:
	@echo "=== Test 3a: Stack variable free (should exit with error) ==="
	-./test3a
	@echo
	@echo "=== Test 3b: Offset pointer free (should exit with error) ==="
	-./test3b
	@echo
	@echo "=== Test 3c: Double free (should exit with error) ==="
	-./test3c

# Clean up
clean:
	rm -f memtest memtest-leak memtest-real memgrind test1 test2 test3a test3b test3c test4 test5 *.o

# Phony targets
.PHONY: all test test-errors clean