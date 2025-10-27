CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

SRCDIR = src
TESTDIR = tests

all: spell

spell: $(SRCDIR)/spell.c
	$(CC) $(CFLAGS) -o spell $(SRCDIR)/spell.c

clean:
	rm -f spell *.o
	
test: spell
	@echo "Running basic test..."
	-./spell $(TESTDIR)/dict_test.txt $(TESTDIR)/input_test.txt
	@echo ""
	@echo "Running case sensitivity test..."
	-./spell $(TESTDIR)/dict_case.txt $(TESTDIR)/input_case.txt

.PHONY: all clean test