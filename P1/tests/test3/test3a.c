#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

int main() {
    printf("Test 3: Error detection\n");
    
    printf("  Testing free() with stack variable...\n");
    printf("  This should produce an error message and exit:\n");
    
    int stack_var = 42;
    printf("  Calling free(&stack_var)...\n");
    fflush(stdout);
    
    free(&stack_var);
    
    return 1;
}