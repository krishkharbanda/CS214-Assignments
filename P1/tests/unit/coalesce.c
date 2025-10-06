#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "mymalloc.h"

int main(void) {
    printf("Running coalesce::adjacent_frees_merge_to_satisfy_larger_request...\n");

    enum { MEMSIZE = 4096, OBJECTS = 2 };
    const size_t objsize = MEMSIZE / OBJECTS - HEADERSIZE; // payload size each

    void *a = malloc(objsize);
    assert(a != NULL && "first malloc failed unexpectedly");
    void *b = malloc(objsize);
    assert(b != NULL && "second malloc failed unexpectedly");

    // Free both; they should be adjacent and coalesce (allocator-dependent)
    free(a);
    free(b);

    // Now a single allocation of combined size should succeed iff coalescing worked
    void *c = malloc(2 * objsize);
    assert(c != NULL && "Expected coalesced block to satisfy 2*objsize");

    free(c);
    printf("✓ passed\n");
    return 0;
}
