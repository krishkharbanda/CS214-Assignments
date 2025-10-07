#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mymalloc.h"

int main() {
    printf("Test 4: Edge cases\n");
    
    // very large allocation
    printf("  Testing huge allocation...\n");
    void* huge = malloc(10000);
    if (huge == NULL) {
        printf("  Large alloc failed as expected\n");
    } else {
        printf("  ERROR: Large alloc should have failed\n");
        free(huge);
        return 1;
    }
    
    // exact size allocation
    printf("  Testing exact size alloc...\n");
    
    void* p1 = malloc(1000);
    void* p2 = malloc(1000);
    void* p3 = malloc(1000);
    
    if (!p1 || !p2 || !p3) {
        printf("  ERROR: Initial allocs failed\n");
        return 1;
    }
    
    free(p2); // free middle
    
    void* exact = malloc(992); // try exact size
    if (exact != NULL) {
        printf("  Exact alloc worked\n");
        free(exact);
    } else {
        printf("  WARNING: Exact alloc failed\n");
    }
    
    free(p1);
    free(p3);
    
    // many tiny allocations
    printf("  Testing tiny allocs...\n");
    
    void* tiny[200];
    int count = 0;
    
    for (int i = 0; i < 200; i++) {
        tiny[i] = malloc(1);
        if (tiny[i] != NULL) {
            count++;
        } else {
            break;
        }
    }
    
    printf("  Got %d tiny objects\n", count);
    
    // free tiny allocations
    for (int i = 0; i < count; i++) {
        if (tiny[i] != NULL) {
            free(tiny[i]);
        }
    }
    
    // alignment test
    printf("  Testing alignment...\n");
    
    void* ptrs[10];
    int align_errors = 0;
    
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(i + 1);
        if (ptrs[i] != NULL) {
            if ((uintptr_t)ptrs[i] % 8 != 0) {
                printf("  ERROR: ptr[%d] not aligned\n", i);
                align_errors++;
            }
        }
    }
    
    if (align_errors == 0) {
        printf("  All pointers aligned\n");
    } else {
        printf("  %d alignment errors\n", align_errors);
    }
    
    for (int i = 0; i < 10; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
    
    // repeated cycles
    printf("  Testing repeated cycles...\n");
    
    for (int cycle = 0; cycle < 10; cycle++) {
        void* ptr = malloc(100);
        if (ptr == NULL) {
            printf("  ERROR: Cycle %d failed\n", cycle);
            return 1;
        }
        
        // write and check data
        memset(ptr, cycle & 0xFF, 100);
        
        int error = 0;
        for (int i = 0; i < 100; i++) {
            if ((unsigned char)((char*)ptr)[i] != (cycle & 0xFF)) {
                error = 1;
                break;
            }
        }
        
        if (error) {
            printf("  ERROR: Data corruption in cycle %d\n", cycle);
            return 1;
        }
        
        free(ptr);
    }
    
    printf("  All cycles OK\n");
    
    printf("Test 4 passed!\n");
    return 0;
}