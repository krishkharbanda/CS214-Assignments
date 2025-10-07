#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main() {
    printf("Test 5: Leak detection\n");
    printf("This will leak memory on purpose.\n");
    
    // allocate without freeing
    void* ptr1 = malloc(100);
    void* ptr2 = malloc(200);
    void* ptr3 = malloc(50);
    
    if (!ptr1 || !ptr2 || !ptr3) {
        printf("ERROR: malloc failed\n");
        return 1;
    }
    
    printf("Allocated 3 objects (100, 200, 50 bytes).\n");
    printf("Expected leak: 350 bytes in 3 objects\n");
    printf("Ending - leak detector should run...\n");
    
    // don't free - let leak detector catch them
    return 0;
}