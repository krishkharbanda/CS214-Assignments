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
            // EOF
            if (total == 0) {
                free(buffer);
                return NULL;
            }
            buffer[total] = '\0';
            return buffer;
        }
        
        if (buffer[total] == '\n') {
            buffer[total] = '\0';
            return buffer;
        }
        
        total++;
    }
    
    // Line too long
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
        // Print prompt in interactive mode
        if (is_interactive) {
            printf("mysh> ");
            fflush(stdout);
        }
        
        // Read command from input
        line = read_command(input_fd);
        if (line == NULL) {
            // EOF reached
            break;
        }
        
        // Tokenize the command
        tokens = tokenize(line);
        if (tokens == NULL || tokens->count == 0) {
            free(line);
            free_tokens(tokens);
            continue;
        }
        
        // Parse into job
        job = parse_job(tokens, &conditional_type);
        if (job == NULL) {
            // Syntax error - skip to next command
            free(line);
            free_tokens(tokens);
            continue;
        }
        
        // Check conditional execution
        if (conditional_type == TOKEN_AND && exit_status != 0) {
            // Don't execute if previous failed
            free(line);
            free_tokens(tokens);
            free_job(job);
            continue;
        }
        
        if (conditional_type == TOKEN_OR && exit_status == 0) {
            // Don't execute if previous succeeded
            free(line);
            free_tokens(tokens);
            free_job(job);
            continue;
        }
        
        // Execute the job
        if (execute_job(job, exit_status, &exit_status) == -1) {
            // die command was executed
            should_exit = 1;
        }
        
        // Check for exit command
        if (job->segment_count > 0) {
            const char *first_cmd = job->segments[0].argv[0];
            if (strcmp(first_cmd, "exit") == 0) {
                should_exit = 1;
            }
        }
        
        // Cleanup
        free(line);
        free_tokens(tokens);
        free_job(job);
    }
}