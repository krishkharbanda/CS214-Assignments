#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mymalloc.h"

static inline size_t r8(size_t k) { return (k + 7) & ~((size_t)7); }

void test_split_large_block_into_two_usable_blocks(void) {
    printf("Running split_and_fragmentation::split_large_block_into_two_usable_blocks...\n");

    // Allocate a block big enough to be split later
    size_t big = r8(256);
    unsigned char *p = malloc(big);
    assert(p != NULL && "initial malloc failed");
    memset(p, 0xCC, big);
    free(p);

    // Now allocate two smaller blocks that should both fit via splitting
    size_t a = r8(80), b = r8(64);
    void *x = malloc(a);
    assert(x != NULL && "first split allocation failed");
    void *y = malloc(b);
    assert(y != NULL && "second split allocation failed");

    free(x);
    free(y);

    printf("✓ passed\n");
}

void test_avoid_splinters_when_remainder_too_small(void) {
    printf("Running split_and_fragmentation::avoid_splinters_when_remainder_too_small...\n");

    // If we request almost the whole heap, remainder should be too small to split,
    // so a follow-up tiny alloc should fail.
    const size_t nearly_all = 4096 - HEADERSIZE - 4;  // leave < HEADER+8
    void *p = malloc(nearly_all);
    assert(p != NULL && "malloc(nearly_all) failed unexpectedly");

    // This should fail because there shouldn't be a valid chunk left to split off
    void *q = malloc(8);
    assert(q == NULL && "Allocator created an unusable splinter");

    free(p);

    printf("✓ passed\n");
}

int main(void) {
    test_split_large_block_into_two_usable_blocks();
    test_avoid_splinters_when_remainder_too_small();

    printf("\nAll split_and_fragmentation tests passed successfully.\n");
    return 0;
}
