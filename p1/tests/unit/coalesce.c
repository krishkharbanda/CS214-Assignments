// tests/unit/coalesce.c
#include <criterion/criterion.h>
#include "mymalloc.h"

Test(coalesce, adjacent_frees_merge_to_satisfy_larger_request) {
    // Use two equal-sized objects modeled after professor's math
    enum { MEMSIZE = 4096, OBJECTS = 2 };
    const size_t objsize = MEMSIZE / OBJECTS - HEADERSIZE; // payload size each

    void *a = malloc(objsize);
    cr_assert_not_null(a);
    void *b = malloc(objsize);
    cr_assert_not_null(b);

    // Free both; they should be adjacent and coalesce
    free(a);
    free(b);

    // Now a single allocation of combined size should succeed iff coalescing worked
    void *c = malloc(2 * objsize);
    cr_assert_not_null(c, "Expected coalesced block to satisfy 2*objsize");

    free(c);
}
