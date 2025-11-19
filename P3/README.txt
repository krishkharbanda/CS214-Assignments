CS 214 Project III: mysh
Authors:
---------
Name: Krish Kharbanda
NetID: kk1297

Name: Ritenkumar Patel
NetID: rp1315

Overview:
---------
mysh is a Unix-like command-line shell developed for CS 214 at Rutgers.
It supports normal and batch modes, pipelines, file redirection, built-in commands (cd, pwd, which, exit, die), and conditional execution (and, or).

Directory Structure:
--------------------
P3/
  include/
    mysh.h
  src/
    main.c
    shell.c
    parse.c
    exec.c
  tests/
    test_basic.sh
    test_builtins.sh
    test_exec.sh
    test_conditionals.sh
    test_redir_pipe.sh
    test_edge_cases.sh
  Makefile
  AUTHOR
  README.txt

Building:
---------
To compile the shell, from the root directory:
    make all

To clean build artifacts:
    make clean

To run all tests (recommended):
    make test

Usage:
------
Interactive shell:
    ./mysh

Batch/script mode:
    ./mysh tests/test_basic.sh

Piped input:
    echo "echo hi" | ./mysh

Features:
---------
- Interactive Mode: Welcome/exit messages, prompt (mysh>), runs user entries.
- Batch Mode: Executes commands from file or piped stdin, no prompts/messages.
- Built-in Commands: cd, pwd, which, exit, die.
- Command Execution: External programs looked up by path or in /usr/local/bin, /usr/bin, /bin.
- Pipelines & Redirection: Arbitrary pipelines (`|`), output (`>`), input (`<`), both can be combined.
- Conditionals: `and` (run if previous succeeded), `or` (run if previous failed), chainable.
- Error Handling: Bad syntax, failed redirection, missing files, wrong built-in usage, command not found, all reported clearly.
- POSIX-Compliant I/O: Uses read(), fork(), execv(), pipe(), dup2(), and standard system calls as per the spec.

Testing:
--------
Key scripts are located in 'tests/' directory. You may run:
    ./mysh tests/test_basic.sh
    ./mysh tests/test_builtins.sh
    ./mysh tests/test_exec.sh
    ./mysh tests/test_conditionals.sh
    ./mysh tests/test_redir_pipe.sh
    ./mysh tests/test_edge_cases.sh

These cover:
    - all built-ins (cd, pwd, which, exit, die)
    - path and argument handling
    - I/O redirection (input, output, both)
    - single and multi-stage pipelines
    - program not found and bad input failures
    - comment and empty-line handling
    - pipes with built-ins
    - conditional logic (and/or, chained, after fail)
    - edge cases (long command, non-existent files...)

Notes & Known Limitations:
--------------------------
- No background execution (`&`)
- No globbing or wildcard expansion
- Arguments are not quoted/escaped (simple splitting)
- Built-ins only participate in pipelines at segment ends
- Error messages use UNIX conventions
