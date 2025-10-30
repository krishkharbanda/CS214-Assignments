CS 214 Project II: Spelling Checker
====================================

Authors:
--------
Name: [Your Name]
NetID: [your_netid]

Name: [Partner Name]
NetID: [partner_netid]

Design Overview:
----------------

This spelling checker uses a hash table for efficient dictionary lookups with O(1) average-case performance. The implementation follows all POSIX requirements and handles edge cases in word processing.

Key Design Decisions:
- Hash table with 50,000 buckets using chaining for collision resolution
- djb2 hash function for good distribution
- Dictionary words stored in lowercase with capitalization flags
- 8KB buffer for efficient file reading using only read()
- Recursive directory traversal with proper filtering

Word Processing Rules:
- Skip words containing only digits or only non-letter characters
- Strip trailing punctuation (!,?.:;@#$% etc)
- Strip leading opening punctuation (quotes, parentheses, brackets)
- Preserve mid-word punctuation (hyphens, apostrophes, etc)

Case Sensitivity:
- Lowercase dictionary words match ANY capitalization in input
- Capitalized dictionary words require capital first letter in input
- Example: dict has "foo" and "Bar"
  - Input "Foo", "FOO", "foo" all match "foo" ✓
  - Input "Bar", "BAR" match "Bar" ✓
  - Input "bar" does NOT match "Bar" ✗

Implementation Details:
-----------------------

File I/O:
- Uses only POSIX read() for all file input (required)
- Uses fprintf/printf for output (permitted)
- 8KB buffer with manual line parsing
- Tracks line/column position during parsing

Directory Traversal:
- Recursively scans all subdirectories
- Skips files/directories starting with '.'
- Only processes files matching specified suffix (default: .txt)
- Uses opendir(), readdir(), closedir()

Error Handling:
- Returns EXIT_FAILURE if any file cannot be opened
- Returns EXIT_FAILURE if any spelling errors found
- Returns EXIT_SUCCESS only if all files opened and no errors found
- Prints error messages to stderr for file access issues

Test Plan:
----------

Our testing strategy covers all major functionality and edge cases:

Test 1: Basic Functionality
----------------------------
Purpose: Verify basic word matching works correctly
Dictionary: tests/dict_basic.txt
  Contents: hello, world, this, is, a, test, program, works, correctly
Input: tests/input_basic.txt
  Contents: "Hello world! This is a test."
           "This program works correctly."
Command: ./spell tests/dict_basic.txt tests/input_basic.txt
Expected: No output (all words found in dictionary)
Tests: Basic lookup, lowercase dict matching any case

Test 2: Case Sensitivity
-------------------------
Purpose: Verify capitalization rules are enforced correctly
Dictionary: tests/dict_case.txt
  Contents: foo, Bar, hello, World
Input: tests/input_case.txt
  Contents: "foo Foo FOO bar Bar BAR"
           "hello Hello HELLO world World WORLD"
Command: ./spell tests/dict_case.txt tests/input_case.txt
Expected Output:
  1:13 bar
  2:25 world
Explanation:
  - "foo", "Foo", "FOO" all match lowercase "foo" ✓
  - "bar" doesn't match capitalized "Bar" ✗
  - "Bar", "BAR" match capitalized "Bar" ✓
  - "hello", "Hello", "HELLO" match lowercase "hello" ✓
  - "world" doesn't match capitalized "World" ✗
  - "World", "WORLD" match capitalized "World" ✓
Tests: Case-sensitive vs case-insensitive matching

Test 3: Punctuation Handling
-----------------------------
Purpose: Verify punctuation is stripped correctly
Dictionary: tests/dict_punct.txt
  Contents: word, hello, test, amazing
Input: tests/input_punct.txt
  Contents: "word! word? (word) 'word' \"word\" word."
           "hello, hello; hello: [hello] {hello}"
           "test... test!!! test???"
           "amazing!!!"
Command: ./spell tests/dict_punct.txt tests/input_punct.txt
Expected: No output (all punctuation stripped correctly)
Tests: Leading quote/paren removal, trailing punctuation removal

Test 4: Word Skipping
---------------------
Purpose: Verify words are skipped appropriately
Dictionary: tests/dict_skip.txt
  Contents: test, word, skidoo, super, i18n
Input: tests/input_skip.txt
  Contents: "test 123 !@# $100 42"
           "23-skidoo super8 i18n"
           "word 1,2023!84-39"
Command: ./spell tests/dict_skip.txt tests/input_skip.txt
Expected: 2:1 23-skidoo
          2:11 super8
Explanation:
  - "123", "!@#", "$100", "42", "1,2023!84-39" skipped (no letters)
  - "23-skidoo" checked (isn't in dictionary) 
  - "super8" checked (isn't in dictionary) ✓
  - "i18n" checked (is in dictionary) ✓
Tests: All-digit words, all-symbol words, mixed words

Test 5: Multiple Files
----------------------
Purpose: Verify filename reporting with multiple files
Dictionary: tests/dict_multi.txt
  Contents: file, one, correct
Input Files:
  tests/input_multi1.txt: "file one"
  tests/input_multi2.txt: "file one correct"
  tests/input_multi3.txt: "file one wrong"
Command: ./spell tests/dict_multi.txt tests/input_multi1.txt tests/input_multi2.txt tests/input_multi3.txt
Expected Output:
  tests/input_multi3.txt:1:10 wrong
Tests: Multiple file handling, filename in output

Test 6: Directory Traversal
----------------------------
Purpose: Verify recursive directory processing
Dictionary: tests/dict_dir.txt
  Contents: directory, file, recursive, test
Directory Structure:
  tests/dirtest/
    file1.txt: "directory file test"
    file2.txt: "recursive file test"
    .hidden.txt: "this should be skipped" (should be ignored)
    subdir/
      file3.txt: "file recursive"
Setup Commands:
  mkdir -p tests/dirtest/subdir
  cp dirtest_file1.txt tests/dirtest/file1.txt
  cp dirtest_file2.txt tests/dirtest/file2.txt
  cp dirtest_hidden.txt tests/dirtest/.hidden.txt
  cp dirtest_subdir_file3.txt tests/dirtest/subdir/file3.txt
Command: ./spell tests/dict_dir.txt tests/dirtest
Expected: No output (all words in dictionary, .hidden.txt skipped)
Tests: Recursive traversal, hidden file skipping, subdirectories

Test 7: Empty File
------------------
Purpose: Verify empty files don't cause errors
Dictionary: tests/dict_empty.txt
  Contents: word
Input: tests/input_empty.txt
  Contents: (empty file)
Command: ./spell tests/dict_empty.txt tests/input_empty.txt
Expected: No output
Tests: Empty file handling

Test 8: Error Cases
-------------------
Purpose: Verify error handling

8a. Missing dictionary:
Command: ./spell nonexistent.txt tests/input_basic.txt
Expected: Error message to stderr, EXIT_FAILURE

8b. Missing input file:
Command: ./spell tests/dict_basic.txt nonexistent.txt
Expected: Error message to stderr, EXIT_FAILURE

8c. Invalid -s usage:
Command: ./spell -s tests/dict_basic.txt
Expected: Error message, EXIT_FAILURE

Running All Tests:
------------------

Compile:
  make

Run all tests:
  make test-all

Run individual tests:
  make test1    # Basic functionality
  make test2    # Case sensitivity
  make test3    # Punctuation
  make test4    # Word skipping
  make test5    # Multiple files
  make test6    # Directory traversal
  make test7    # Empty file
  make test8    # Error cases

Clean:
  make clean

Directory Structure:
--------------------

P2/
├── src/
│   └── spell.c          # Main implementation
├── tests/
│   ├── dict_basic.txt   # Test 1 dictionary
│   ├── input_basic.txt  # Test 1 input
│   ├── dict_case.txt    # Test 2 dictionary
│   ├── input_case.txt   # Test 2 input
│   ├── dict_punct.txt   # Test 3 dictionary
│   ├── input_punct.txt  # Test 3 input
│   ├── dict_skip.txt    # Test 4 dictionary
│   ├── input_skip.txt   # Test 4 input
│   ├── dict_multi.txt   # Test 5 dictionary
│   ├── input_multi*.txt # Test 5 inputs
│   ├── dict_dir.txt     # Test 6 dictionary
│   ├── dirtest/         # Test 6 directory structure
│   ├── dict_empty.txt   # Test 7 dictionary
│   └── input_empty.txt  # Test 7 input
├── Makefile
├── AUTHOR
└── README

Known Limitations:
------------------
- Maximum word length: 256 characters
- Hash table size fixed at 50,000 buckets
- No Unicode normalization support
- Case checking only examines first character
- Path length limited to 1024 characters

Performance:
------------
- Hash table provides O(1) average lookup
- Dictionary loading: O(n) where n = number of words
- Text processing: O(m) where m = number of words in input
- Memory usage: O(n) for dictionary storage
- Efficient for large dictionaries (tested with 100K+ words)

Exit Codes:
-----------
EXIT_SUCCESS (0): All files opened successfully AND no spelling errors found
EXIT_FAILURE (1): File could not be opened OR spelling errors were found