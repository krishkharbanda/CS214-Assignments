Here is a concise, **single README** file that integrates condensed content from all three markdown docs:

***

# MyShell (`mysh`) - CS 214 Project III

## Overview
`mysh` is a fully-featured command-line shell for Unix-like systems written in C. It supports interactive and batch modes, pipelines, I/O redirection, built-in commands, and conditionals. The architecture and all code are guided by Rutgers CS 214 lectures and official references, using only POSIX and ANSI C APIs.

***

## Features

### Modes
- **Interactive**: Prompt (`mysh> `), welcome/exit messages, runs user input
- **Batch**: Reads commands from script file or stdin, no prompt/messages

### Core Shell Loop
- Always uses low-level `read()` (unbuffered), accumulating until newline
- Tokenizes commands, supports comments (`#`)
- Single command parser for both modes

### Syntax & Execution
- Recognizes tokens: words, `<`, `>`, `|`, `and`, `or`
- Supports:
  - **Pipelines** (`|`): Arbitrary-length, correct fd wiring/closing
  - **Redirection** (`<`, `>`): Only after arguments; proper open/dup2/error
  - **Conditionals**: `and` and `or` chaining by previous exit status
- **Built-ins**: `cd`, `pwd`, `which`, `exit`, `die`—directly implemented per spec
- **External commands**: Only searches `/usr/local/bin`, `/usr/bin`, `/bin` (or full path)

### Error Handling
- Syntax errors: Printed, command skipped
- File/exec errors: perror-style, prints message and fails the command
- All commands exit with correct status code for conditionals

***

## File Structure

- `main.c` - Entry; CLI args, isatty(), mode selection, main loop call, SIGCHLD setup
- `shell.c` - Main command loop; prompt, input, tokenization, execution, conditionals
- `parse.c` - Tokenization, parsing, dynamic arrays/list for arguments/pipes
- `exec.c` - Built-in/external execution, redirection, pipes, waitpid, program lookup
- `mysh.h` - All structures, enums, and function prototypes
- `Makefile` - Build (all, clean, test)
- `test_basic.sh`, `test_pipes.sh` - Batch test scripts

***

## Compilation & Usage

Build:
```sh
make all
```
Clean:
```sh
make clean
```
Run interactively:
```sh
./mysh
```
Run with script:
```sh
./mysh script.sh
```
Piped input:
```sh
echo 'echo hello' | ./mysh
```

***

## Example Commands

- `cd /tmp`               # changes directory
- `pwd`                   # prints current directory
- `echo hello > out.txt`  # output redirection
- `cat < out.txt`         # input redirection
- `ls | head -3`          # pipeline
- `and echo yes`          # only if previous succeeded
- `or echo fail`          # only if previous failed
- `# this is a comment`   # ignored

***

## Test Plan (Condensed)

1. **Basic**: `echo`, `pwd`, `ls`, `which`
2. **Built-ins**: `cd`, `which`, `exit`, `die`
3. **Redirections**: Test `<` and `>`, combined
4. **Pipes**: Simple and multi-segment
5. **Conditionals**: `and`, `or`, chains
6. **Comments & Syntax**: Comments, bad syntax errors
7. **Interactive/UI**: Prompt, welcome/exit, EOF
8. **Batch**: No prompts/messages
9. **Edge cases**: Empty commands, spaces, EOF, non-existent files

See `TEST_PLAN.md` for full cases.

***

## Architecture

- **Input:** `main.c` → `shell_loop()` (prompt, read using `read()`, tokenize, parse, execute)
- **Parse:** Tokenizes input (including handling comments), builds a list of job segments
- **Exec:** Handles I/O redirection, pipes (via `pipe()`, `dup2()`), fork/execv for external (proper path logic), implements built-ins directly
- **Conditionals:** Status of each command saved; next command runs or skips per syntax (`and`/`or`)
- **Memory:** Dynamic allocation everywhere, with `free_*` used after every command

***

## Known Limitations

- No support for background execution (`&`)
- No globbing or wildcard expansion
- No quoting/escaping arguments (whitespace and tokenization is naive)
- Builtins run in parent; cannot be redirected/piped unless at pipeline ends
- No command history or tab-completion

***

## Submission

- Update README with both partner names and NetIDs!
- Verify it compiles and passes tests on iLab/GCC 9.3+
- Pack all `.c`, `.h`, `Makefile`, `README`, and test files as tar.gz for submission

Example:
```sh
tar -czf mysh.tar.gz *.c *.h Makefile README.md test_*.sh
```

***

## Authors
*Partner 1 Name (NetID1)  
Partner 2 Name (NetID2)*

*CS 214 Systems Programming, Fall 2025, Rutgers University*