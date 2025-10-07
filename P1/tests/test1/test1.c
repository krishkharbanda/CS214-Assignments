#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

int main() {
    printf("Test 1: Basic allocation and data integrity\n");
    
    printf("  Testing basic allocation...\n");
    char* ptr1 = malloc(10);
    if (ptr1 == NULL) {
        printf("  ERROR: malloc(10) returned NULL\n");
        return 1;
    }
    
    char* ptr2 = malloc(20);
    if (ptr2 == NULL) {
        printf("  ERROR: malloc(20) returned NULL\n");
        return 1;
    }
    
    char* ptr3 = malloc(30);
    if (ptr3 == NULL) {
        printf("  ERROR: malloc(30) returned NULL\n");
        return 1;
    }
    
    printf("  Testing data integrity...\n");
    memset(ptr1, 0xAA, 10);
    memset(ptr2, 0xBB, 20);
    memset(ptr3, 0xCC, 30);
    
    int errors = 0;
    for (int i = 0; i < 10; i++) {
        if ((unsigned char)ptr1[i] != 0xAA) {
            printf("  ERROR: ptr1[%d] = 0x%02X, expected 0xAA\n", i, (unsigned char)ptr1[i]);
            errors++;
        }
    }
    
    for (int i = 0; i < 20; i++) {
        if ((unsigned char)ptr2[i] != 0xBB) {
            printf("  ERROR: ptr2[%d] = 0x%02X, expected 0xBB\n", i, (unsigned char)ptr2[i]);
            errors++;
        }
    }
    
    for (int i = 0; i < 30; i++) {
        if ((unsigned char)ptr3[i] != 0xCC) {
            printf("  ERROR: ptr3[%d] = 0x%02X, expected 0xCC\n", i, (unsigned char)ptr3[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("  PASS: Data integrity maintained\n");
    } else {
        printf("  FAIL: %d data corruption errors found\n", errors);
    }
    
    printf("  Testing basic free...\n");
    free(ptr1);
    free(ptr2);
    free(ptr3);
    printf("  PASS: Basic free operations completed\n");
    
    printf("  Testing allocation after free...\n");
    char* ptr4 = malloc(15);
    if (ptr4 == NULL) {
        printf("  ERROR: malloc(15) after free returned NULL\n");
        return 1;
    }
    free(ptr4);
    printf("  PASS: Allocation after free works\n");
    
    printf("  Testing zero-size allocation...\n");
    char* ptr_zero = malloc(0);
    if (ptr_zero != NULL) {
        printf("  WARNING: malloc(0) returned non-NULL pointer\n");
        free(ptr_zero);
    } else {
        printf("  PASS: malloc(0) returned NULL\n");
    }
    
    printf("  Testing free(NULL)...\n");
    free(NULL);
    printf("  PASS: free(NULL) completed without error\n");
    
    printf("Test 1 completed successfully!\n");
    return 0;
}