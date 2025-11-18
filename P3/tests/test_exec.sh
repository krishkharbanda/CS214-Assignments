# === TEST 2: EXTERNAL EXECUTION AND PATHING ===

# [cite_start]2.1 Test Bare Name Search (Success) [cite: 123, 124]
echo "--- 2.1: BARE NAME SEARCH ---"
echo Test successful bare name search

# [cite_start]2.2 Test Pathname Execution (Success) [cite: 122]
echo "--- 2.2: PATHNAME EXECUTION ---"
/bin/echo Test successful pathname execution

# 2.3 Test Program Search Order (Implicit)
# This test assumes the system has 'which' in one of the search paths
echo "--- 2.3: SEARCH ORDER (SHOULD FIND LS) ---"
[cite_start]which ls # Verifies search order: /usr/local/bin, /usr/bin, /bin [cite: 124]

# 2.4 Test Argument Passing
echo "--- 2.4: ARGUMENT PASSING ---"
ls -a -l # Tests multiple arguments

# [cite_start]2.5 Test Redirection of Standard Input for child process in batch mode [cite: 16]
# This test ensures 'cat' is launched with stdin redirected to /dev/null in batch mode.
# If it were in interactive mode, it would block waiting for input.
echo "--- 2.5: CHILD STDIN REDIRECTION (BATCH) ---"
/bin/cat # Should immediately exit and not block

# 2.6 Test Program Not Found (Failure)
echo "--- 2.6: PROGRAM NOT FOUND (FAILURE) ---"
non_existent_program_mysh_2
# [cite_start]mysh must print an error and FAIL the command [cite: 116]

# 2.7 Test Comments
echo "--- 2.7: COMMENTS AND EMPTY COMMANDS ---"
# [cite_start]This is a comment [cite: 63]
echo Should appear after comment command