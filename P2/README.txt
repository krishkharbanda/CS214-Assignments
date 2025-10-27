CS 214 Project II: Spelling Checker
====================================

Authors:
--------
Name: Krish Kharbanda
NetID: kk1297

Name: Riten Patel
NetID: rp1315

Design:
-------

Implementation uses a hash table for fast dictionary lookups. The dictionary stores words in normalized (lowercase) form with a flag indicating if the original word had a capital first letter.

Key components:
- Hash table with chaining for collision resolution (50,000 buckets)
- Dictionary loader using read() for POSIX compliance
- Word normalization: strips leading opening punctuation and trailing punctuation
- Capitalization handling: lowercase dict words match any case, capitalized dict words require capital first letter
- Recursive directory traversal with file suffix filtering
- Line and column tracking for accurate error reporting

Word Processing:
- Skips words with only digits or only symbols
- Strips trailing punctuation (!,?.:;@#$% etc)
- Strips leading quotes, parentheses, brackets
- Preserves mid-word punctuation (hyphens, apostrophes)

File I/O:
- Uses only read() for file input (POSIX requirement)
- 8KB buffer for efficient reading
- Tracks line/column position while parsing

Directory Traversal:
- Recursively scans directories
- Skips hidden files/directories (starting with '.')
- Only processes files matching specified suffix (default .txt)

Test Plan:
----------

Requirements Tested:

1. Dictionary Loading
   - Simple word list
   - Duplicates in dictionary
   - Mixed case words
   - Empty lines

2. Word Matching
   - Exact matches
   - Case-insensitive matching (lowercase dict entries)
   - Case-sensitive matching (capitalized dict entries)
   - Words with trailing punctuation
   - Words with leading punctuation
   - Words with mid-word punctuation

3. Word Skipping
   - All-digit words (should skip)
   - All-symbol words (should skip)
   - Mixed letter/digit words (should check)

4. File Processing
   - Single file
   - Multiple files
   - Directory with .txt files
   - Nested directories
   - Hidden files (should skip)
   - Non-.txt files in directory (should skip)

5. Error Handling
   - Missing dictionary file
   - Missing input file
   - Unreadable files
   - Invalid -s usage

6. Edge Cases
   - Empty files
   - Files with no newline at end
   - Very long words
   - Unicode characters
   - Mixed line endings

Test Scenarios:
---------------

Test 1: Basic functionality
- Dictionary: dict1.txt (contains: hello, world, test)
- Input: test1.txt (contains: "Hello world! This is a test.")
- Expected: No errors (all words in dictionary)

Test 2: Case sensitivity
- Dictionary: dict2.txt (contains: foo, Bar)
- Input: test2.txt (contains: "foo Foo FOO bar Bar BAR")
- Expected: "bar" flagged (Bar is capitalized in dict, bar is not valid)

Test 3: Punctuation handling
- Dictionary: dict3.txt (contains: word)
- Input: test3.txt (contains: "word! word? (word) 'word' word.")
- Expected: No errors (all punctuation stripped correctly)

Test 4: Word skipping
- Dictionary: dict4.txt (contains: test)
- Input: test4.txt (contains: "test 123 !@# test123")
- Expected: Only "test123" may be flagged if not in dict

Test 5: Directory traversal
- Dictionary: dict5.txt
- Directory structure:
  testdir/
    file1.txt
    file2.txt
    subdir/
      file3.txt
    .hidden.txt (should skip)
    file.doc (should skip)
- Expected: Processes file1.txt, file2.txt, file3.txt only

Test 6: Multiple files
- Dictionary: dict6.txt
- Files: file1.txt, file2.txt, file3.txt
- Expected: Errors reported with filename:line:col format

Test 7: Standard input
- Dictionary: dict7.txt
- Input: pipe text via stdin
- Expected: Errors reported as line:col (no filename)

Test 8: Error conditions
- Non-existent dictionary
- Non-existent input file
- Directory without read permissions
- Expected: Appropriate error messages, EXIT_FAILURE

Running Tests:
--------------

make
./spell dict1.txt test1.txt
./spell dict2.txt testdir/
./spell -s .md dict3.txt docs/
cat input.txt | ./spell dict4.txt

Expected Output Format:
-----------------------

For errors:
filename:line:column word

For single file or stdin:
line:column word

Exit Codes:
-----------
EXIT_SUCCESS (0): All files opened successfully, no spelling errors
EXIT_FAILURE (1): File open error OR spelling errors found

Known Limitations:
------------------
- Maximum word length: 256 characters
- Hash table size fixed at 50,000 buckets
- No support for Unicode normalization
- Case checking only examines first character

Implementation Notes:
---------------------
- Uses djb2 hash function for good distribution
- Chaining handles hash collisions
- Dictionary stored once in memory (not modified after loading)
- File processing uses 8KB buffer for efficiency
- All string operations use safe bounded functions