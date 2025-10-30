CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

SRCDIR = src
TESTDIR = tests

all: spell

spell: $(SRCDIR)/spell.c
	$(CC) $(CFLAGS) -o spell $(SRCDIR)/spell.c

setup-dirtest:
	@mkdir -p $(TESTDIR)/dirtest/subdir
	@cp dirtest_file1.txt $(TESTDIR)/dirtest/file1.txt 2>/dev/null || true
	@cp dirtest_file2.txt $(TESTDIR)/dirtest/file2.txt 2>/dev/null || true
	@cp dirtest_hidden.txt $(TESTDIR)/dirtest/.hidden.txt 2>/dev/null || true
	@cp dirtest_subdir_file3.txt $(TESTDIR)/dirtest/subdir/file3.txt 2>/dev/null || true

test1: spell
	@echo " Test 1: Basic Functionality "
	@echo "Should produce no output (all words correct)"
	-./spell $(TESTDIR)/dict_basic.txt $(TESTDIR)/input_basic.txt
	@echo ""

test2: spell
	@echo " Test 2: Case Sensitivity "
	@echo "Should flag 'bar' and 'world' as incorrect"
	-./spell $(TESTDIR)/dict_case.txt $(TESTDIR)/input_case.txt
	@echo ""

test3: spell
	@echo " Test 3: Punctuation Handling "
	@echo "Should produce no output (punctuation handled correctly)"
	-./spell $(TESTDIR)/dict_punct.txt $(TESTDIR)/input_punct.txt
	@echo ""

test4: spell
	@echo " Test 4: Word Skipping "
	@echo "Should flag '23-skidoo' and 'super8' as incorrect"
	-./spell $(TESTDIR)/dict_skip.txt $(TESTDIR)/input_skip.txt
	@echo ""

test5: spell
	@echo " Test 5: Multiple Files "
	@echo "Should flag 'wrong' in input_multi3.txt"
	-./spell $(TESTDIR)/dict_multi.txt $(TESTDIR)/input_multi1.txt $(TESTDIR)/input_multi2.txt $(TESTDIR)/input_multi3.txt
	@echo ""

test6: spell setup-dirtest
	@echo " Test 6: Directory Traversal "
	@echo "Should produce no output (all words correct, .hidden skipped)"
	-./spell $(TESTDIR)/dict_dir.txt $(TESTDIR)/dirtest
	@echo ""

test7: spell
	@echo " Test 7: Empty File "
	@echo "Should produce no output (empty file handled)"
	-./spell $(TESTDIR)/dict_empty.txt $(TESTDIR)/input_empty.txt
	@echo ""

test8: spell
	@echo " Test 8: Error Handling "
	@echo "8a. Missing dictionary file:"
	-./spell nonexistent.txt $(TESTDIR)/input_basic.txt 2>&1 | head -1
	@echo ""
	@echo "8b. Missing input file:"
	-./spell $(TESTDIR)/dict_basic.txt nonexistent.txt 2>&1 | head -1
	@echo ""
	@echo "8c. Invalid -s usage:"
	-./spell -s $(TESTDIR)/dict_basic.txt 2>&1 | head -1
	@echo ""

# Run all tests
test-all: test1 test2 test3 test4 test5 test6 test7 test8
	@echo " All Tests Complete "

clean:
	rm -f spell *.o
	rm -rf $(TESTDIR)/dirtest

.PHONY: all setup-dirtest test1 test2 test3 test4 test5 test6 test7 test8 test-all test-quick clean