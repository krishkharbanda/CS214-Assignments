#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main() {
    printf("Test 3c: Error detection - double free\n");
    
    printf("  Allocating memory...\n");
    int* ptr = malloc(sizeof(int) * 100);
    int* q = ptr;
    
    if (ptr == NULL) {
        printf("  ERROR: malloc failed\n");
        return 1;
    }
    
    printf("  First free - should work...\n");
    free(ptr);
    printf("  First free done\n");
    
    printf("  Second free - should error and exit:\n");
    fflush(stdout);
    
    free(q); // should error and exit
    
    printf("  ERROR: Should have exited\n");
    return 1;
}