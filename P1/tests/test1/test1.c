#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

int main() {
    printf("Test 1: Basic malloc/free\n");
    
    // basic allocation
    printf("  Testing basic allocation...\n");
    char* ptr1 = malloc(10);
    if (ptr1 == NULL) {
        printf("  ERROR: malloc(10) failed\n");
        return 1;
    }
    
    char* ptr2 = malloc(20);
    if (ptr2 == NULL) {
        printf("  ERROR: malloc(20) failed\n");
        return 1;
    }
    
    char* ptr3 = malloc(30);
    if (ptr3 == NULL) {
        printf("  ERROR: malloc(30) failed\n");
        return 1;
    }
    
    // test data integrity
    printf("  Testing data integrity...\n");
    memset(ptr1, 0xAA, 10);
    memset(ptr2, 0xBB, 20);
    memset(ptr3, 0xCC, 30);
    
    int errors = 0;
    for (int i = 0; i < 10; i++) {
        if ((unsigned char)ptr1[i] != 0xAA) {
            printf("  ERROR: ptr1[%d] corrupted\n", i);
            errors++;
        }
    }
    
    for (int i = 0; i < 20; i++) {
        if ((unsigned char)ptr2[i] != 0xBB) {
            printf("  ERROR: ptr2[%d] corrupted\n", i);
            errors++;
        }
    }
    
    for (int i = 0; i < 30; i++) {
        if ((unsigned char)ptr3[i] != 0xCC) {
            printf("  ERROR: ptr3[%d] corrupted\n", i);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("  Data integrity OK\n");
    } else {
        printf("  %d corruption errors\n", errors);
    }
    
    // free memory
    printf("  Testing free...\n");
    free(ptr1);
    free(ptr2);
    free(ptr3);
    printf("  Free operations OK\n");
    
    // allocation after free
    printf("  Testing alloc after free...\n");
    char* ptr4 = malloc(15);
    if (ptr4 == NULL) {
        printf("  ERROR: malloc after free failed\n");
        return 1;
    }
    free(ptr4);
    printf("  Alloc after free OK\n");
    
    // zero size allocation
    printf("  Testing malloc(0)...\n");
    char* ptr_zero = malloc(0);
    if (ptr_zero != NULL) {
        printf("  WARNING: malloc(0) returned non-NULL\n");
        free(ptr_zero);
    } else {
        printf("  malloc(0) returned NULL\n");
    }
    
    // free NULL
    printf("  Testing free(NULL)...\n");
    free(NULL);
    printf("  free(NULL) OK\n");
    
    printf("Test 1 passed!\n");
    return 0;
}