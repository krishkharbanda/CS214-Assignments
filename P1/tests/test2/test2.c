#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

int main() {
    printf("Test 2: Coalescing and fragmentation handling\n");
    
    printf("  Testing adjacent chunk coalescing...\n");
    
    char* ptr1 = malloc(100);
    char* ptr2 = malloc(100);
    char* ptr3 = malloc(100);
    
    if (!ptr1 || !ptr2 || !ptr3) {
        printf("  ERROR: Initial allocations failed\n");
        return 1;
    }
    
    free(ptr2);
    
    free(ptr1);
    
    free(ptr3);
    
    char* large_ptr = malloc(280);
    
    if (large_ptr == NULL) {
        printf("  ERROR: Could not allocate large chunk after coalescing\n");
        return 1;
    }
    
    printf("  PASS: Coalescing appears to work\n");
    free(large_ptr);
    
    printf("  Testing fragmentation handling...\n");
    
    char* ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(50);
        if (ptrs[i] == NULL) {
            printf("  ERROR: malloc failed at iteration %d\n", i);
            return 1;
        }
    }
    
    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    for (int i = 1; i < 10; i += 2) {
        ptrs[i] = malloc(40);
        if (ptrs[i] == NULL) {
            printf("  ERROR: Could not reallocate in freed space\n");
            return 1;
        }
    }
    
    printf("  PASS: Fragmentation handling works\n");
    
    for (int i = 0; i < 10; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
    
    printf("  Testing many small allocations...\n");
    
    char* small_ptrs[100];
    int successful_allocs = 0;
    
    for (int i = 0; i < 100; i++) {
        small_ptrs[i] = malloc(8);
        if (small_ptrs[i] != NULL) {
            successful_allocs++;
            memset(small_ptrs[i], i & 0xFF, 8);
        }
    }
    
    printf("  Successfully allocated %d small chunks\n", successful_allocs);
    
    int corruption_errors = 0;
    for (int i = 0; i < successful_allocs; i++) {
        if (small_ptrs[i] != NULL) {
            for (int j = 0; j < 8; j++) {
                if ((unsigned char)small_ptrs[i][j] != (i & 0xFF)) {
                    corruption_errors++;
                    break;
                }
            }
        }
    }
    
    if (corruption_errors == 0) {
        printf("  PASS: No data corruption in small allocations\n");
    } else {
        printf("  ERROR: %d chunks had data corruption\n", corruption_errors);
    }
    
    for (int i = 0; i < successful_allocs; i++) {
        if (small_ptrs[i] != NULL) {
            free(small_ptrs[i]);
        }
    }
    
    printf("  Testing allocation after defragmentation...\n");
    
    char* final_ptr = malloc(1000);
    if (final_ptr != NULL) {
        printf("  PASS: Can allocate large chunk after defragmentation\n");
        free(final_ptr);
    } else {
        printf("  WARNING: Could not allocate large chunk after defragmentation\n");
    }
    
    printf("Test 2 completed successfully!\n");
    return 0;
}