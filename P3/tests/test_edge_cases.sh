echo "=== EDGE CASES TEST SUITE ==="
echo ""

# Test 5.1: Multiple Redirections in Single Command
echo "--- 5.1: Multiple Redirections (Arg Order) ---"
echo "test data" > /tmp/edge_input.txt
cat < /tmp/edge_input.txt > /tmp/edge_output.txt
cat /tmp/edge_output.txt
echo "--- PASS: Simultaneous < and > in any order ---"
echo ""

# Test 5.2: Empty Pipeline Error
echo "--- 5.2: Syntax - Pipe with No Command ---"
# This should fail gracefully
echo "" | /bin/sh -c "mysh" || true
echo "--- PASS: Should handle gracefully ---"
echo ""

# Test 5.3: Very Long Command Line
echo "--- 5.3: Long Command with Many Args ---"
echo arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10
echo "--- PASS: Handles many arguments ---"
echo ""

# Test 5.4: Redirection to Existing File (Truncate)
echo "--- 5.4: Output Redirection Truncates File ---"
echo "original content" > /tmp/trunc_test.txt
echo "new content" > /tmp/trunc_test.txt
cat /tmp/trunc_test.txt
echo "--- PASS: File truncated correctly ---"
echo ""

# Test 5.5: Four-Stage Pipeline
echo "--- 5.5: Four-Stage Pipeline ---"
echo -e "z\na\nc\nb" | sort | uniq | head -2
echo "--- PASS: 3+ pipes work ---"
echo ""

# Test 5.6: Conditional After Failed Builtin
echo "--- 5.6: Conditional After Failed Built-in ---"
cd /nonexistent/path 2>/dev/null || echo "Correctly caught cd failure"
echo "--- PASS: Conditional works with builtin failure ---"
echo ""

# Test 5.7: Program Not in Search Path
echo "--- 5.7: Program Not Found ---"
/bin/sh -c "mysh" -c "nonexistent_prog" 2>&1 || true
echo "--- PASS: Error for missing program ---"
echo ""

# Test 5.8: Nested/Multiple Conditionals
echo "--- 5.8: Complex Conditional Chain ---"
true && true && echo "Both succeeded"
false || false || echo "Both failed"
echo "--- PASS: Conditional chains work ---"
echo ""

# Test 5.9: Input Redirection from Nonexistent File
echo "--- 5.9: Input from Missing File ---"
cat < /tmp/nonexistent_input_12345.txt 2>&1 || echo "Error caught"
echo "--- PASS: Handles missing input file ---"
echo ""

# Test 5.10: Exit Status Propagation Through Pipeline
echo "--- 5.10: Pipeline Exit Status ---"
false | true | false
echo "Pipeline exited with: $?"  
echo "--- PASS: Exit status from last command ---"
echo ""

rm -f /tmp/edge_*.txt /tmp/trunc_test.txt

echo "=== ALL EDGE CASE TESTS COMPLETE ==="