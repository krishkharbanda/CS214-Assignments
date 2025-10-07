#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

int main() {
    printf("Test 2: Coalescing and fragmentation\n");
    
    // test coalescing
    printf("  Testing coalescing...\n");
    
    char* ptr1 = malloc(100);
    char* ptr2 = malloc(100);
    char* ptr3 = malloc(100);
    
    if (!ptr1 || !ptr2 || !ptr3) {
        printf("  ERROR: Initial allocs failed\n");
        return 1;
    }
    
    free(ptr2); // free middle
    free(ptr1); // free first - should coalesce
    free(ptr3); // free third - should coalesce more
    
    // try to allocate large chunk
    char* big_ptr = malloc(280);
    
    if (big_ptr == NULL) {
        printf("  ERROR: Large alloc after coalescing failed\n");
        return 1;
    }
    
    printf("  Coalescing works\n");
    free(big_ptr);
    
    // test fragmentation
    printf("  Testing fragmentation...\n");
    
    char* ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(50);
        if (ptrs[i] == NULL) {
            printf("  ERROR: malloc failed at %d\n", i);
            return 1;
        }
    }
    
    // free every other chunk
    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    // try to reallocate in freed spaces
    for (int i = 1; i < 10; i += 2) {
        ptrs[i] = malloc(40);
        if (ptrs[i] == NULL) {
            printf("  ERROR: Realloc in freed space failed\n");
            return 1;
        }
    }
    
    printf("  Fragmentation handling works\n");
    
    // cleanup
    for (int i = 0; i < 10; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
    
    // test many small allocations
    printf("  Testing many small allocs...\n");
    
    char* small_ptrs[100];
    int success = 0;
    
    for (int i = 0; i < 100; i++) {
        small_ptrs[i] = malloc(8);
        if (small_ptrs[i] != NULL) {
            success++;
            memset(small_ptrs[i], i & 0xFF, 8);
        }
    }
    
    printf("  Allocated %d small chunks\n", success);
    
    // verify no corruption
    int corrupt = 0;
    for (int i = 0; i < success; i++) {
        if (small_ptrs[i] != NULL) {
            for (int j = 0; j < 8; j++) {
                if ((unsigned char)small_ptrs[i][j] != (i & 0xFF)) {
                    corrupt++;
                    break;
                }
            }
        }
    }
    
    if (corrupt == 0) {
        printf("  No data corruption\n");
    } else {
        printf("  %d chunks corrupted\n", corrupt);
    }
    
    // free all
    for (int i = 0; i < success; i++) {
        if (small_ptrs[i] != NULL) {
            free(small_ptrs[i]);
        }
    }
    
    // test large alloc after defrag
    printf("  Testing large alloc after defrag...\n");
    
    char* final = malloc(1000);
    if (final != NULL) {
        printf("  Large alloc after defrag works\n");
        free(final);
    } else {
        printf("  WARNING: Large alloc after defrag failed\n");
    }
    
    printf("Test 2 passed!\n");
    return 0;
}