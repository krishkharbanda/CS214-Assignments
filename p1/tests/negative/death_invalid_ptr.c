#include <stdio.h>
#include "mymalloc.h"

int main(void) {
    int x = 42;
    // Freeing a foreign pointer: must print error and exit(2)
    free(&x);
    puts("ERROR: invalid free did not exit");
    return 0;
}
