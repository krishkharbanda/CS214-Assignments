#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main() {
    printf("Test 3a: Error detection - stack variable\n");
    
    printf("  Testing free() with stack var...\n");
    printf("  Should print error and exit:\n");
    
    int x = 42;
    printf("  Calling free(&x)...\n");
    fflush(stdout);
    
    free(&x); // should error and exit
    
    printf("  ERROR: Should have exited\n");
    return 1;
}