#include <criterion/criterion.h>
#include <string.h>
#include "mymalloc.h"

static inline size_t r8(size_t k) { return (k + 7) & ~((size_t)7); }

Test(split_and_fragmentation, split_large_block_into_two_usable_blocks) {
    // Allocate a block big enough to be split later
    size_t big = r8(256);
    unsigned char *p = malloc(big);
    cr_assert_not_null(p);
    memset(p, 0xCC, big);

    free(p);

    // Now allocate two smaller blocks that should both fit via splitting
    size_t a = r8(80), b = r8(64);
    void *x = malloc(a);
    cr_assert_not_null(x, "first split allocation failed");
    void *y = malloc(b);
    cr_assert_not_null(y, "second split allocation failed");

    free(x);
    free(y);
}

Test(split_and_fragmentation, avoid_splinters_when_remainder_too_small) {
    // If we request almost the whole heap, remainder should be too small to split,
    // so a follow-up tiny alloc should fail.
    // Heap size is 4096, initial free payload is 4096 - header at heap start.
    const size_t nearly_all = 4096 - /*initial header*/ HEADERSIZE - 4; // leave < HEADER+8
    void *p = malloc(nearly_all);
    cr_assert_not_null(p);

    // This should fail because there shouldn't be a valid chunk left to split off
    void *q = malloc(8);
    cr_assert_null(q, "Allocator created an unusable splinter");

    free(p);
}
