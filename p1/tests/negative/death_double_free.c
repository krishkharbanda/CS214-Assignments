// tests/negative/death_double_free.c
#include <stdio.h>
#include "mymalloc.h"

int main(void) {
    void *p = malloc(16);
    if (!p) return 1;
    free(p);
    // This must print to stderr and exit(2)
    free(p);
    // Should not reach here
    puts("ERROR: double free did not exit");
    return 0;
}
