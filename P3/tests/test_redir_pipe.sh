# === TEST 3: REDIRECTION AND PIPELINES ===

# [cite_start]3.1 Test Output Redirection (>) [cite: 68, 149]
echo "--- 3.1: OUTPUT REDIRECTION (>) ---"
echo Hello to file > output_test_1.txt
/bin/cat output_test_1.txt

# [cite_start]3.2 Test Input Redirection (<) [cite: 68, 145]
echo "--- 3.2: INPUT REDIRECTION (<) ---"
echo Read me from file > input_test_1.txt
/bin/cat < input_test_1.txt

# [cite_start]3.3 Test Simultaneous Redirection and Argument Order [cite: 163, 165]
echo "--- 3.3: SIMULTANEOUS REDIRECTION ---"
/bin/cat < input_test_1.txt > output_test_2.txt
/bin/cat output_test_2.txt
# Should be equivalent to:
/bin/cat > output_test_3.txt < input_test_1.txt
/bin/cat output_test_3.txt

# [cite_start]3.4 Test Output Redirection Failure (e.g., to a directory) [cite: 151]
echo "--- 3.4: REDIRECTION FAILURE (TO DIR) ---"
[cite_start]echo Failed > /bin # This should fail (cannot open file in dir) [cite: 151]

# [cite_start]3.5 Test Simple Pipe (One pipe, two processes) [cite: 71, 155]
echo "--- 3.5: SIMPLE PIPE ---"
echo One two three | wc -w # Should output 3

# [cite_start]3.6 Test Multi-Pipe (Two pipes, three processes) [cite: 155]
echo "--- 3.6: MULTI-PIPE ---"
ls /bin | head -n 5 | tail -n 1 # Should output the 5th program name in /bin

# [cite_start]3.7 Test Built-in in a Pipe [cite: 139]
echo "--- 3.7: BUILT-IN IN PIPE ---"
pwd | wc -c # Counts characters in the path

# [cite_start]3.8 Test Pipeline Success/Failure (Last command determines status) [cite: 114, 158]
echo "--- 3.8: PIPE SUCCESS/FAILURE ---"
/bin/true | /bin/false # The whole pipeline FAILS
/bin/false | /bin/true # The whole pipeline SUCCEEDS