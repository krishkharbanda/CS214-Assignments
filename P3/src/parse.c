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
    
    tokens->items = malloc(100 * sizeof(Token));
    tokens->capacity = 100;
    tokens->count = 0;
    
    if (tokens->items == NULL) {
        perror("malloc");
        free(tokens);
        return NULL;
    }
    
    // Remove leading whitespace
    while (*line && isspace(*line)) line++;
    
    // Skip comments
    if (*line == '#') {
        return tokens;
    }
    
    // Find comment position
    const char *comment_pos = strchr(line, '#');
    int line_len = comment_pos ? (comment_pos - line) : strlen(line);
    
    char *temp_line = malloc(line_len + 1);
    if (temp_line == NULL) {
        perror("malloc");
        free(tokens->items);
        free(tokens);
        return NULL;
    }
    
    strncpy(temp_line, line, line_len);
    temp_line[line_len] = '\0';
    
    int i = 0;
    while (temp_line[i]) {
        while (temp_line[i] && isspace(temp_line[i])) i++;
        
        if (temp_line[i] == '\0') break;
        
        Token token;
        
        if (temp_line[i] == '<') {
            token.type = TOKEN_REDIRECT_IN;
            token.value = malloc(2);
            token.value[0] = '<';
            token.value[1] = '\0';
            i++;
        } else if (temp_line[i] == '>') {
            token.type = TOKEN_REDIRECT_OUT;
            token.value = malloc(2);
            token.value[0] = '>';
            token.value[1] = '\0';
            i++;
        } else if (temp_line[i] == '|') {
            token.type = TOKEN_PIPE;
            token.value = malloc(2);
            token.value[0] = '|';
            token.value[1] = '\0';
            i++;
        } else {
            int start = i;
            while (temp_line[i] && !isspace(temp_line[i]) && 
                   temp_line[i] != '<' && temp_line[i] != '>' && temp_line[i] != '|') {
                i++;
            }
            
            int len = i - start;
            token.value = malloc(len + 1);
            strncpy(token.value, temp_line + start, len);
            token.value[len] = '\0';
            
            if (strcmp(token.value, "and") == 0) {
                token.type = TOKEN_AND;
            } else if (strcmp(token.value, "or") == 0) {
                token.type = TOKEN_OR;
            } else {
                token.type = TOKEN_WORD;
            }
        }
        
        if (tokens->count >= tokens->capacity) {
            tokens->capacity *= 2;
            Token *new_items = realloc(tokens->items, tokens->capacity * sizeof(Token));
            if (new_items == NULL) {
                perror("realloc");
                free(temp_line);
                free_tokens(tokens);
                return NULL;
            }
            tokens->items = new_items;
        }
        
        tokens->items[tokens->count++] = token;
    }
    
    free(temp_line);
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
    
    job->segments = malloc(50 * sizeof(JobSegment));
    if (job->segments == NULL) {
        perror("malloc");
        free(job);
        return NULL;
    }
    
    job->segment_count = 0;
    int token_idx = 0;
    
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
        segment.argv = malloc(100 * sizeof(char *));
        segment.argc = 0;
        
        if (segment.argv == NULL) {
            perror("malloc");
            free_job(job);
            return NULL;
        }
        
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
                segment.output_file = tokens->items[token_idx].value;
                token_idx++;
            } else if (t->type == TOKEN_WORD) {
                segment.argv[segment.argc++] = t->value;
                token_idx++;
            } else {
                fprintf(stderr, "Syntax error in pipeline\n");
                free(segment.argv);
                free_job(job);
                return NULL;
            }
        }
        
        if (segment.argc == 0) {
            fprintf(stderr, "Syntax error: empty command\n");
            free(segment.argv);
            free_job(job);
            return NULL;
        }
        
        segment.argv[segment.argc] = NULL;
        
        job->segments[job->segment_count++] = segment;
        
        if (token_idx < tokens->count && tokens->items[token_idx].type == TOKEN_PIPE) {
            token_idx++;
        }
    }
    
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
        free(job->segments[i].argv);
    }
    
    free(job->segments);
    free(job);
}