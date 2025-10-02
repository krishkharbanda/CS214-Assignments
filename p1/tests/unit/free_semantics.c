// tests/unit/free_semantics.c
#include <criterion/criterion.h>
#include <string.h>
#include "mymalloc.h"

Test(free_semantics, free_null_is_noop) {
    free(NULL);
    // If we got here, it's a pass (no crash, no exit)
    cr_assert(true);
}

Test(free_semantics, allocator_does_not_modify_payload_while_allocated) {
    const size_t n = 64;
    unsigned char *p = malloc(n);
    cr_assert_not_null(p);
    for (size_t i = 0; i < n; ++i) p[i] = (unsigned char)(i ^ 0x5A);

    // Perform some unrelated alloc/free to exercise the allocator
    void *q = malloc(32); cr_assert_not_null(q);
    free(q);

    // Ensure our payload stayed intact
    for (size_t i = 0; i < n; ++i)
        cr_expect_eq(p[i], (unsigned char)(i ^ 0x5A), "payload modified at %zu", i);

    free(p);
}
