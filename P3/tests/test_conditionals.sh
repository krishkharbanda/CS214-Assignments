# TEST 4: CONDITIONAL EXECUTION

# 4.1 AND Success 
echo "--- 4.1: AND (SUCCESS) ---"
/bin/true
and echo "SUCCESS: AND ran after true"

# 4.2 AND Failure 
echo "--- 4.2: AND (FAILURE) ---"
/bin/false
and echo "FAILURE: AND was skipped after false (this should NOT print)"

# 4.3 OR Failure 
echo "--- 4.3: OR (FAILURE) ---"
/bin/false
or echo "SUCCESS: OR ran after false"

# 4.4 OR Success 
echo "--- 4.4: OR (SUCCESS) ---"
/bin/true
or echo "FAILURE: OR was skipped after true (this should NOT print)"

# 4.5 Chained Conditionals
echo "--- 4.5: CHAINED OR ---"
/bin/false
or /bin/false
or echo "CHAINED: Ran only after two failures"

# 4.6 Conditional Built-in Failure 
echo "--- 4.6: CONDITIONAL BUILT-IN FAILURE ---"
cd invalid_dir
or echo "BUILT-IN FAIL: Ran after 'cd' failed"

# 4.7 Test EXIT and DIE 
echo "--- 4.7: CONDITIONAL DIE ---"
/bin/false
or die "FATAL ERROR: mysh should now terminate with failure" 
exit