#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mysh.h"

TokenArray *tokenize(const char *line) {
    TokenArray *tokens = malloc(sizeof(TokenArray));
    if (tokens == NULL) {
        perror("malloc");
        return NULL;
    }
    
    // Initial allocation for tokens
    tokens->items = malloc(100 * sizeof(Token));
    tokens->capacity = 100;
    tokens->count = 0;
    
    if (tokens->items == NULL) {
        perror("malloc");
        free(tokens);
        return NULL;
    }
    
    // Skip leading whitespace
    while (*line && isspace(*line)) line++;
    
    // Skip command if it starts with a comment
    if (*line == '#') {
        return tokens;
    }
    
    // Find comment position to trim the line
    const char *comment_pos = strchr(line, '#');
    
    // Determine the length of the line content before any comment or EOL
    int line_len = comment_pos ? (comment_pos - line) : strlen(line);
    
    // Allocate and copy the clean line content (tokens only)
    char *line_copy = malloc(line_len + 1);
    if (line_copy == NULL) {
        perror("malloc");
        free_tokens(tokens);
        return NULL;
    }
    
    strncpy(line_copy, line, line_len);
    line_copy[line_len] = '\0';
    
    char *saveptr;
    // Use whitespace characters as delimiters for strtok_r
    char *token_str = strtok_r(line_copy, " \t\n", &saveptr);
    
    while (token_str != NULL) {
        Token token;
        
        // --- 1. Identify Special Tokens ---
        // Note: The assignment permits assuming >|< are separated by whitespace.
        if (strcmp(token_str, "<") == 0) {
            token.type = TOKEN_REDIRECT_IN;
        } else if (strcmp(token_str, ">") == 0) {
            token.type = TOKEN_REDIRECT_OUT;
        } else if (strcmp(token_str, "|") == 0) {
            token.type = TOKEN_PIPE;
        } else {
            // --- 2. Identify Word/Conditional Tokens ---
            token.type = TOKEN_WORD;
            
            if (strcmp(token_str, "and") == 0) {
                token.type = TOKEN_AND;
            } else if (strcmp(token_str, "or") == 0) {
                token.type = TOKEN_OR;
            }
        }
        
        // Allocate and copy the value
        token.value = strdup(token_str);
        if (token.value == NULL) {
            perror("strdup");
            free(line_copy);
            free_tokens(tokens);
            return NULL;
        }
        
        // Handle dynamic array expansion
        if (tokens->count >= tokens->capacity) {
            tokens->capacity *= 2;
            Token *new_items = realloc(tokens->items, tokens->capacity * sizeof(Token));
            if (new_items == NULL) {
                perror("realloc");
                free(line_copy);
                free_tokens(tokens);
                return NULL;
            }
            tokens->items = new_items;
        }
        
        tokens->items[tokens->count++] = token;
        
        // Get the next token
        token_str = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    free(line_copy);
    return tokens;
}

void free_tokens(TokenArray *tokens) {
    if (tokens == NULL) return;
    
    for (int i = 0; i < tokens->count; i++) {
        free(tokens->items[i].value);
    }
    
    free(tokens->items);
    free(tokens);
}

Job *parse_job(TokenArray *tokens, int *conditional_type) {
    *conditional_type = 0;
    
    Job *job = malloc(sizeof(Job));
    if (job == NULL) {
        perror("malloc");
        return NULL;
    }
    
    // Initial allocation for job segments
    job->segments = malloc(50 * sizeof(JobSegment));
    if (job->segments == NULL) {
        perror("malloc");
        free(job);
        return NULL;
    }
    
    job->segment_count = 0;
    int token_idx = 0;
    
    // Check for leading conditional (and/or)
    if (token_idx < tokens->count && 
        (tokens->items[token_idx].type == TOKEN_AND || 
         tokens->items[token_idx].type == TOKEN_OR)) {
        *conditional_type = tokens->items[token_idx].type;
        token_idx++;
    }
    
    while (token_idx < tokens->count) {
        JobSegment segment;
        segment.input_file = NULL;
        segment.output_file = NULL;
        // Initial allocation for argv
        segment.argv = malloc(100 * sizeof(char *));
        segment.argc = 0;
        
        if (segment.argv == NULL) {
            perror("malloc");
            free_job(job);
            return NULL;
        }
        
        // Process tokens within a single pipeline segment
        while (token_idx < tokens->count && tokens->items[token_idx].type != TOKEN_PIPE) {
            Token *t = &tokens->items[token_idx];
            
            if (t->type == TOKEN_REDIRECT_IN) {
                token_idx++;
                if (token_idx >= tokens->count || tokens->items[token_idx].type != TOKEN_WORD) {
                    fprintf(stderr, "Syntax error: < requires filename\n");
                    free(segment.argv);
                    free_job(job);
                    return NULL;
                }
                // Store the filename pointer, which was allocated in tokenize
                segment.input_file = tokens->items[token_idx].value;
                token_idx++;
            } else if (t->type == TOKEN_REDIRECT_OUT) {
                token_idx++;
                if (token_idx >= tokens->count || tokens->items[token_idx].type != TOKEN_WORD) {
                    fprintf(stderr, "Syntax error: > requires filename\n");
                    free(segment.argv);
                    free_job(job);
                    return NULL;
                }
                // Store the filename pointer
                segment.output_file = tokens->items[token_idx].value;
                token_idx++;
            } else if (t->type == TOKEN_WORD) {
                // Add argument to argv list
                segment.argv[segment.argc++] = t->value;
                token_idx++;
            } else {
                // Should not happen if tokenizing is correct (conditional tokens are checked first)
                fprintf(stderr, "Syntax error in pipeline\n");
                free(segment.argv);
                free_job(job);
                return NULL;
            }
        }
        
        // Check for empty command in segment (e.g., "ls | | grep")
        if (segment.argc == 0) {
            fprintf(stderr, "Syntax error: empty command\n");
            free(segment.argv);
            free_job(job);
            return NULL;
        }
        
        // Null-terminate the argument list for execv()
        segment.argv[segment.argc] = NULL;
        
        job->segments[job->segment_count++] = segment;
        
        // Advance past the pipe token, if present
        if (token_idx < tokens->count && tokens->items[token_idx].type == TOKEN_PIPE) {
            token_idx++;
        }
    }
    
    // Final check: if the input was empty after conditional check
    if (job->segment_count == 0) {
        fprintf(stderr, "Syntax error: no command\n");
        free(job->segments);
        free(job);
        return NULL;
    }
    
    return job;
}

void free_job(Job *job) {
    if (job == NULL) return;
    
    for (int i = 0; i < job->segment_count; i++) {
        // Free the argv array for each segment
        free(job->segments[i].argv);
    }
    
    free(job->segments);
    free(job);
}