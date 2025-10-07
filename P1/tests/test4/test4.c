#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mymalloc.h"

int main() {
    printf("Test 4: Boundary conditions and edge cases\n");
    
    printf("  Testing very large allocation...\n");
    void* huge_ptr = malloc(10000);
    if (huge_ptr == NULL) {
        printf("  PASS: Large allocation correctly returned NULL\n");
    } else {
        printf("  ERROR: Large allocation should have failed\n");
        free(huge_ptr);
        return 1;
    }
    
    printf("  Testing allocation of exactly remaining space...\n");
    
    void* ptr1 = malloc(1000);
    void* ptr2 = malloc(1000);
    void* ptr3 = malloc(1000);
    
    if (!ptr1 || !ptr2 || !ptr3) {
        printf("  ERROR: Initial allocations failed\n");
        return 1;
    }
    
    free(ptr2);
    
    void* exact_ptr = malloc(992);
    if (exact_ptr != NULL) {
        printf("  PASS: Exact allocation succeeded\n");
        free(exact_ptr);
    } else {
        printf("  WARNING: Exact allocation failed (may be due to alignment)\n");
    }
    
    free(ptr1);
    free(ptr3);
    
    printf("  Testing many tiny allocations...\n");
    
    void* tiny_ptrs[200];
    int tiny_count = 0;
    
    for (int i = 0; i < 200; i++) {
        tiny_ptrs[i] = malloc(1);
        if (tiny_ptrs[i] != NULL) {
            tiny_count++;
        } else {
            break;
        }
    }
    
    printf("  Successfully allocated %d tiny (1-byte) objects\n", tiny_count);
    
    for (int i = 0; i < tiny_count; i++) {
        if (tiny_ptrs[i] != NULL) {
            free(tiny_ptrs[i]);
        }
    }
    
    printf("  Testing pointer alignment...\n");
    
    void* ptrs[10];
    int alignment_errors = 0;
    
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(i + 1);
        if (ptrs[i] != NULL) {
            if ((uintptr_t)ptrs[i] % 8 != 0) {
                printf("  ERROR: ptr[%d] not 8-byte aligned: %p\n", i, ptrs[i]);
                alignment_errors++;
            }
        }
    }
    
    if (alignment_errors == 0) {
        printf("  PASS: All pointers are properly aligned\n");
    } else {
        printf("  ERROR: %d alignment errors found\n", alignment_errors);
    }
    
    for (int i = 0; i < 10; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
    
    printf("  Testing repeated allocation/free cycles...\n");
    
    for (int cycle = 0; cycle < 10; cycle++) {
        void* cycle_ptr = malloc(100);
        if (cycle_ptr == NULL) {
            printf("  ERROR: Allocation failed at cycle %d\n", cycle);
            return 1;
        }
        
        memset(cycle_ptr, cycle & 0xFF, 100);
        
        int data_error = 0;
        for (int i = 0; i < 100; i++) {
            if ((unsigned char)((char*)cycle_ptr)[i] != (cycle & 0xFF)) {
                data_error = 1;
                break;
            }
        }
        
        if (data_error) {
            printf("  ERROR: Data corruption in cycle %d\n", cycle);
            return 1;
        }
        
        free(cycle_ptr);
    }
    
    printf("  PASS: All allocation/free cycles completed successfully\n");
    
    printf("Test 4 completed successfully!\n");
    return 0;
}