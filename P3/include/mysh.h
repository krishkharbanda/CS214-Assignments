#ifndef MYSH_H
#define MYSH_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

// Array for tokens
typedef struct {
    Token *items;
    int count;
    int capacity;
} TokenArray;

// Job segment 
typedef struct {
    char **argv;
    int argc;
    char *input_file;
    char *output_file;
} JobSegment;

// Complete job 
typedef struct {
    JobSegment *segments;
    int segment_count;
} Job;

void shell_loop(int input_fd, int is_interactive);

char *read_command(int input_fd);

TokenArray *tokenize(const char *line);
void free_tokens(TokenArray *tokens);
Job *parse_job(TokenArray *tokens, int *conditional_type);
void free_job(Job *job);

int execute_job(Job *job, int last_exit_status, int *new_exit_status, int is_interactive);
int execute_builtin(const char *cmd, char **argv, int argc);
int is_builtin(const char *cmd);

char *find_program(const char *name);
void print_error(const char *msg);

#endif 