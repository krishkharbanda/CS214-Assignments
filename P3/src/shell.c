#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "mysh.h"

#define BUFFER_SIZE 4096

char *read_command(int input_fd) {
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        perror("malloc");
        return NULL;
    }
    
    int total = 0;
    int n;
    
    while (total < BUFFER_SIZE - 1) {
        n = read(input_fd, buffer + total, 1);
        
        if (n < 0) {
            perror("read");
            free(buffer);
            return NULL;
        }
        
        if (n == 0) {
            if (total == 0) {
                free(buffer);
                return NULL;
            }
            buffer[total] = '\0';
            return buffer;
        }
        
        if (buffer[total] == '\n') {
            buffer[total] = '\0'; // Replace newline with null terminator
            return buffer;
        }
        
        total++;
    }
    
    fprintf(stderr, "Command line too long\n");
    free(buffer);
    return NULL;
}

void shell_loop(int input_fd, int is_interactive) {
    char *line = NULL;
    TokenArray *tokens = NULL;
    Job *job = NULL;
    int exit_status = 0;
    int conditional_type = 0;
    int should_exit = 0;
    
    while (!should_exit) {
        if (is_interactive) {
            printf("mysh> ");
            fflush(stdout);
        }
        
        line = read_command(input_fd);
        if (line == NULL) {
            // EOF reached
            break;
        }
        
        tokens = tokenize(line);
        if (tokens == NULL || tokens->count == 0) {
            free(line);
            free_tokens(tokens);
            continue;
        }
        
        job = parse_job(tokens, &conditional_type);
        if (job == NULL) {
            free(line);
            free_tokens(tokens);
            exit_status = 1; 
            continue;
        }
        
        if (conditional_type == TOKEN_AND && exit_status != 0) {
            free(line);
            free_tokens(tokens);
            free_job(job);
            continue;
        }
        
        if (conditional_type == TOKEN_OR && exit_status == 0) {
            free(line);
            free_tokens(tokens);
            free_job(job);
            continue;
        }
        
        if (execute_job(job, exit_status, &exit_status, is_interactive) == -1) {
            should_exit = 1;
        }
        
        if (job->segment_count > 0) {
            const char *first_cmd = job->segments[0].argv[0];
            if (strcmp(first_cmd, "exit") == 0) {
                should_exit = 1;
            }
        }
        
        free(line);
        free_tokens(tokens);
        free_job(job);
    }
}