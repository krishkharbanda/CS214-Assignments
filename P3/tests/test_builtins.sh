# === TEST 1: BUILT-IN COMMANDS ===

# 1.1 Test PWD (Success)
echo "--- 1.1: PWD (SUCCESS) ---"
pwd

# 1.2 Test CD (Success)
echo "--- 1.2: CD (SUCCESS) ---"
mkdir test_dir_1
cd test_dir_1
pwd # Should show current path ending in /test_dir_1
cd ..
pwd # Should show path one level up

# [cite_start]1.3 Test CD (Failure: Directory Not Found) [cite: 131]
echo "--- 1.3: CD (FAILURE: NOT FOUND) ---"
cd directory_does_not_exist
# [cite_start]mysh must print an error and the command must FAIL [cite: 131]
pwd # Should confirm directory didn't change

# [cite_start]1.4 Test CD (Failure: Wrong Arguments) [cite: 131]
echo "--- 1.4: CD (FAILURE: WRONG ARGS) ---"
cd arg1 arg2
# [cite_start]mysh must print an error and the command must FAIL [cite: 131]

# 1.5 Test WHICH (Success: Found)
echo "--- 1.5: WHICH (SUCCESS: FOUND) ---"
[cite_start]which echo # Should print path (e.g., /bin/echo or /usr/bin/echo) [cite: 134]

# 1.6 Test WHICH (Failure: Not Found)
echo "--- 1.6: WHICH (FAILURE: NOT FOUND) ---"
which non_existent_program_mysh
# [cite_start]Should print nothing and FAIL [cite: 135]

# [cite_start]1.7 Test WHICH (Failure: Built-in) [cite: 135]
echo "--- 1.7: WHICH (FAILURE: BUILT-IN) ---"
which cd
# [cite_start]Should print nothing and FAIL [cite: 135]

# 1.8 Test DIE (Must be the last test - it terminates mysh with failure)
# die will be run as the next test using conditionals.