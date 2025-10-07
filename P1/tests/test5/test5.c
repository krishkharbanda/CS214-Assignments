#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main() {
    printf("Test 5: Leak detection test\n");
    printf("This program will intentionally leak memory to test leak detection.\n");
    
    void* ptr1 = malloc(100);
    void* ptr2 = malloc(200);
    void* ptr3 = malloc(50);
    
    if (!ptr1 || !ptr2 || !ptr3) {
        printf("ERROR: malloc failed\n");
        return 1;
    }
    
    printf("Allocated 3 objects (100, 200, 50 bytes) without freeing them.\n");
    printf("Expected leak report: 350 bytes in 3 objects\n");
    printf("Program ending - leak detector should run and report leaks...\n");
    
    return 0;
}