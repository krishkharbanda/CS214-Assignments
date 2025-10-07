#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main() {
    printf("Test 3c: Error detection - double free\n");
    
    printf("  Allocating memory and freeing twice...\n");
    int* ptr = malloc(sizeof(int) * 100);
    int* q = ptr;
    
    if (ptr == NULL) {
        printf("  ERROR: malloc failed\n");
        return 1;
    }
    
    printf("  First free(ptr) - should succeed...\n");
    free(ptr);
    printf("  First free completed\n");
    
    printf("  Second free(q) - should produce error and exit:\n");
    fflush(stdout);
    
    free(q);
    
    return 1;
}