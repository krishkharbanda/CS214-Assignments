#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include "mysh.h"

int is_builtin(const char *cmd) {
    return strcmp(cmd, "cd") == 0 ||
           strcmp(cmd, "pwd") == 0 ||
           strcmp(cmd, "which") == 0 ||
           strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "die") == 0;
}

int execute_builtin(const char *cmd, char **argv, int argc) {
    if (strcmp(cmd, "cd") == 0) {
        if (argc != 2) {
            fprintf(stderr, "cd: wrong number of arguments\n");
            return 1;
        }
        if (chdir(argv[1]) != 0) {
            perror("cd");
            return 1;
        }
        return 0;
    }
    
    if (strcmp(cmd, "pwd") == 0) {
        if (argc != 1) {
            fprintf(stderr, "pwd: wrong number of arguments\n");
            return 1;
        }
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            return 1;
        }
        printf("%s\n", cwd);
        return 0;
    }
    
    if (strcmp(cmd, "which") == 0) {
        if (argc != 2) {
            fprintf(stderr, "which: wrong number of arguments\n");
            return 1;
        }
        
        if (is_builtin(argv[1])) {
            return 1;
        }
        
        char *path = find_program(argv[1]);
        if (path == NULL) {
            return 1;
        }
        printf("%s\n", path);
        free(path);
        return 0;
    }
    
    if (strcmp(cmd, "exit") == 0) {
        return 0;
    }
    
    if (strcmp(cmd, "die") == 0) {
        for (int i = 1; i < argc; i++) {
            if (i > 1) printf(" ");
            printf("%s", argv[i]);
        }
        if (argc > 1) printf("\n");
        return -1;
    }
    
    return 1;
}

char *find_program(const char *name) {
    if (strchr(name, '/') != NULL) {
        if (access(name, X_OK) == 0) {
            char *result = malloc(strlen(name) + 1);
            strcpy(result, name);
            return result;
        }
        return NULL;
    }
    
    const char *paths[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    
    for (int i = 0; i < 3; i++) {
        char *full_path = malloc(strlen(paths[i]) + strlen(name) + 2);
        sprintf(full_path, "%s/%s", paths[i], name);
        
        if (access(full_path, X_OK) == 0) {
            return full_path;
        }
        
        free(full_path);
    }
    
    return NULL;
}

int execute_job(Job *job, int last_exit_status, int *new_exit_status) {
    if (job->segment_count == 0) {
        *new_exit_status = 1;
        return 0;
    }
    
    if (job->segment_count == 1) {
        JobSegment *seg = &job->segments[0];
        
        if (is_builtin(seg->argv[0])) {
            int result = execute_builtin(seg->argv[0], seg->argv, seg->argc);
            
            if (result == -1) {
                *new_exit_status = 1;
                return -1;
            }
            
            *new_exit_status = (result == 0) ? 0 : 1;
            return 0;
        }
        
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork");
            *new_exit_status = 1;
            return 0;
        }
        
        if (pid == 0) {
            if (seg->input_file != NULL) {
                int fd = open(seg->input_file, O_RDONLY);
                if (fd < 0) {
                    perror(seg->input_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            
            if (seg->output_file != NULL) {
                int fd = open(seg->output_file, 
                             O_WRONLY | O_CREAT | O_TRUNC, 
                             0640);
                if (fd < 0) {
                    perror(seg->output_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            char *prog_path = find_program(seg->argv[0]);
            if (prog_path == NULL) {
                fprintf(stderr, "%s: command not found\n", seg->argv[0]);
                exit(EXIT_FAILURE);
            }
            
            execv(prog_path, seg->argv);
            perror("execv");
            exit(EXIT_FAILURE);
        }
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            *new_exit_status = WEXITSTATUS(status);
        } else {
            *new_exit_status = 1;
        }
        
        return 0;
    }
    
    int pipe_count = job->segment_count - 1;
    int pipes[pipe_count][2];
    pid_t pids[job->segment_count];
    
    for (int i = 0; i < pipe_count; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            *new_exit_status = 1;
            return 0;
        }
    }
    
    for (int i = 0; i < job->segment_count; i++) {
        JobSegment *seg = &job->segments[i];
        
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            *new_exit_status = 1;
            return 0;
        }
        
        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            if (i < job->segment_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < pipe_count; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            if (i == 0 && seg->input_file != NULL) {
                int fd = open(seg->input_file, O_RDONLY);
                if (fd < 0) {
                    perror(seg->input_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            
            if (i == job->segment_count - 1 && seg->output_file != NULL) {
                int fd = open(seg->output_file, 
                             O_WRONLY | O_CREAT | O_TRUNC, 
                             0640);
                if (fd < 0) {
                    perror(seg->output_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            char *prog_path = find_program(seg->argv[0]);
            if (prog_path == NULL) {
                fprintf(stderr, "%s: command not found\n", seg->argv[0]);
                exit(EXIT_FAILURE);
            }
            
            execv(prog_path, seg->argv);
            perror("execv");
            exit(EXIT_FAILURE);
        } else {
            pids[i] = pid;
        }
    }
    
    for (int i = 0; i < pipe_count; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    int final_status = 0;
    for (int i = 0; i < job->segment_count; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        
        if (i == job->segment_count - 1) {
            if (WIFEXITED(status)) {
                final_status = WEXITSTATUS(status);
            } else {
                final_status = 1;
            }
        }
    }
    
    *new_exit_status = final_status;
    return 0;
}