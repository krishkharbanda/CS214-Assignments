#ifndef MYSH_H
#define MYSH_H

#include <stdio.h>
#include <stdlib.h>

// Token types for parsing
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *value;
} Token;

// Dynamic array for tokens
typedef struct {
    Token *items;
    int count;
    int capacity;
} TokenArray;

// Job segment (one command in a pipeline)
typedef struct {
    char **argv;
    int argc;
    char *input_file;
    char *output_file;
} JobSegment;

// Complete job (one or more segments in a pipeline)
typedef struct {
    JobSegment *segments;
    int segment_count;
} Job;

// Function prototypes - main loop
void shell_loop(int input_fd, int is_interactive);

// Function prototypes - input handling
char *read_command(int input_fd);

// Function prototypes - parsing
TokenArray *tokenize(const char *line);
void free_tokens(TokenArray *tokens);
Job *parse_job(TokenArray *tokens, int *conditional_type);
void free_job(Job *job);

// Function prototypes - execution
int execute_job(Job *job, int last_exit_status, int *new_exit_status);
int execute_builtin(const char *cmd, char **argv, int argc);
int is_builtin(const char *cmd);

// Function prototypes - utility
char *find_program(const char *name);
void print_error(const char *msg);

#endif // MYSH_H