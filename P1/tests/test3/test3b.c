#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main() {
    printf("Test 3b: Error detection - offset pointer\n");
    
    printf("  Allocating memory and attempting to free offset pointer...\n");
    int* ptr = malloc(sizeof(int) * 10);
    
    if (ptr == NULL) {
        printf("  ERROR: malloc failed\n");
        return 1;
    }
    
    printf("  This should produce an error message and exit:\n");
    printf("  Calling free(ptr + 1)...\n");
    fflush(stdout);
    
    free(ptr + 1);

    return 1;
}