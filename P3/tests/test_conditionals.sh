# === TEST 4: CONDITIONAL EXECUTION ===
# [cite_start]Note: /bin/true returns exit status 0 (Success) [cite: 111]
# [cite_start]Note: /bin/false returns non-zero exit status (Failure) [cite: 111]

# [cite_start]4.1 AND Success [cite: 76]
echo "--- 4.1: AND (SUCCESS) ---"
/bin/true
and echo "SUCCESS: AND ran after true"

# [cite_start]4.2 AND Failure [cite: 76]
echo "--- 4.2: AND (FAILURE) ---"
/bin/false
and echo "FAILURE: AND was skipped after false (this should NOT print)"

# [cite_start]4.3 OR Failure [cite: 76]
echo "--- 4.3: OR (FAILURE) ---"
/bin/false
or echo "SUCCESS: OR ran after false"

# [cite_start]4.4 OR Success [cite: 76]
echo "--- 4.4: OR (SUCCESS) ---"
/bin/true
or echo "FAILURE: OR was skipped after true (this should NOT print)"

# [cite_start]4.5 Chained Conditionals (Baz runs only if Foo and Bar FAIL) [cite: 97]
echo "--- 4.5: CHAINED OR ---"
/bin/false
or /bin/false
or echo "CHAINED: Ran only after two failures"

# [cite_start]4.6 Conditional Built-in Failure [cite: 112, 113]
echo "--- 4.6: CONDITIONAL BUILT-IN FAILURE ---"
cd invalid_dir
or echo "BUILT-IN FAIL: Ran after 'cd' failed"

# [cite_start]4.7 Test EXIT and DIE (Must be the last tests) [cite: 136, 137]
echo "--- 4.7: CONDITIONAL DIE ---"
/bin/false
or die "FATAL ERROR: mysh should now terminate with failure" # This should run
exit
# If 'die' executes, mysh terminates here and 'exit' is ignored.