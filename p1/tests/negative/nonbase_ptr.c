// tests/negative/death_nonbase_ptr.c
#include <stdio.h>
#include <stdint.h>
#include "mymalloc.h"

int main(void) {
    unsigned char *p = malloc(32);
    if (!p) return 1;
    // Freeing a non-base address inside a valid allocation must exit(2)
    free(p + 1);
    puts("ERROR: non-base free did not exit");
    return 0;
}
