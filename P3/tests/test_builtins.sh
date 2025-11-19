# TEST 1: BUILT-IN COMMANDS

echo "--- 1.1: PWD (SUCCESS) ---"
pwd

# 1.2 Test CD (Success)
echo "--- 1.2: CD (SUCCESS) ---"
mkdir test_dir_1
cd test_dir_1
pwd # Should show current path ending in /test_dir_1
cd ..
pwd # Should show path one level up

# 1.3 Test CD (Failure: Directory Not Found) [cite: 131]
echo "--- 1.3: CD (FAILURE: NOT FOUND) ---"
cd directory_does_not_exist
# mysh must print an error and the command must FAIL [cite: 131]
pwd # Should confirm directory didn't change

# 1.4 Test CD (Failure: Wrong Arguments) [cite: 131]
echo "--- 1.4: CD (FAILURE: WRONG ARGS) ---"
cd arg1 arg2
# mysh must print an error and the command must FAIL [cite: 131]

# 1.5 Test WHICH (Success: Found)
echo "--- 1.5: WHICH (SUCCESS: FOUND) ---"
which echo # Should print path (e.g., /bin/echo or /usr/bin/echo) [cite: 134]

# 1.6 Test WHICH (Failure: Not Found)
echo "--- 1.6: WHICH (FAILURE: NOT FOUND) ---"
which non_existent_program_mysh
# Should print nothing and FAIL [cite: 135]

# 1.7 Test WHICH (Failure: Built-in) [cite: 135]
echo "--- 1.7: WHICH (FAILURE: BUILT-IN) ---"
which cd
# Should print nothing and FAIL [cite: 135]

# 1.8 Test DIE 
# die will be run as the next test using conditionals.