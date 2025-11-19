# TEST 3: REDIRECTION AND PIPELINES

# 3.1 Test Output Redirection (>)
echo "--- 3.1: OUTPUT REDIRECTION (>) ---"
echo Hello to file > output_test_1.txt
/bin/cat output_test_1.txt

# 3.2 Test Input Redirection (<) 
echo "--- 3.2: INPUT REDIRECTION (<) ---"
echo Read me from file > input_test_1.txt
/bin/cat < input_test_1.txt

# 3.3 Test Simultaneous Redirection and Argument Order
echo "--- 3.3: SIMULTANEOUS REDIRECTION ---"
/bin/cat < input_test_1.txt > output_test_2.txt
/bin/cat output_test_2.txt
# Should be equivalent to:
/bin/cat > output_test_3.txt < input_test_1.txt
/bin/cat output_test_3.txt

# 3.4 Test Output Redirection Failure (e.g., to a directory) 
echo "--- 3.4: REDIRECTION FAILURE (TO DIR) ---"
echo Failed > /bin # This should fail (cannot open file in dir) 

# 3.5 Test Simple Pipe (One pipe, two processes) 
echo "--- 3.5: SIMPLE PIPE ---"
echo One two three | wc -w # Should output 3

# 3.6 Test Multi-Pipe (Two pipes, three processes) 
echo "--- 3.6: MULTI-PIPE ---"
ls /bin | head -n 5 | tail -n 1 # Should output the 5th program name in /bin

# 3.7 Test Built-in in a Pipe [cite: 139]
echo "--- 3.7: BUILT-IN IN PIPE ---"
pwd | wc -c 

# 3.8 Test Pipeline Success/Failure 
echo "--- 3.8: PIPE SUCCESS/FAILURE ---"
/bin/true | /bin/false # The whole pipeline FAILS
/bin/false | /bin/true # The whole pipeline SUCCEEDS