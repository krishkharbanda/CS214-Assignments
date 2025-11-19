#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "mysh.h"

int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    int is_interactive = 0;
    int input_fd = STDIN_FILENO;
    
    if (argc > 2) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    if (argc == 2) {
        // Batch mode: read from file
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            perror("fopen");
            return EXIT_FAILURE;
        }
        input_fd = fileno(input_file);
    }
    
    is_interactive = isatty(input_fd);
    
    signal(SIGCHLD, SIG_IGN);
    
    if (is_interactive) {
        printf("Welcome to my shell!\n");
    }
    
    shell_loop(input_fd, is_interactive);
    
    if (input_file != NULL) {
        fclose(input_file);
    }
    
    if (is_interactive) {
        printf("Exiting my shell.\n");
    }
    
    return EXIT_SUCCESS;
}