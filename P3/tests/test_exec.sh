# TEST 2: EXTERNAL EXECUTION AND PATHING

# 2.1 Test Bare Name Search (Success)
echo "--- 2.1: BARE NAME SEARCH ---"
echo Test successful bare name search

# 2.2 Test Pathname Execution (Success) 
echo "--- 2.2: PATHNAME EXECUTION ---"
/bin/echo Test successful pathname execution

# 2.3 Test Program Search Order 
echo "--- 2.3: SEARCH ORDER (SHOULD FIND LS) ---"
which ls 

# 2.4 Test Argument Passing
echo "--- 2.4: ARGUMENT PASSING ---"
ls -a -l 

# 2.5 Test Redirection of Standard Input for child process in batch mode 
echo "--- 2.5: CHILD STDIN REDIRECTION (BATCH) ---"
/bin/cat # Should immediately exit and not block

# 2.6 Test Program Not Found (Failure)
echo "--- 2.6: PROGRAM NOT FOUND (FAILURE) ---"
non_existent_program_mysh_2

# 2.7 Test Comments
echo "--- 2.7: COMMENTS AND EMPTY COMMANDS ---"
echo Should appear after comment command