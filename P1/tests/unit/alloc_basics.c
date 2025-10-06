#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "mymalloc.h"

static inline size_t round8(size_t k) { return (k + 7) & ~((size_t)7); }

void test_alignment_and_rounding() {
    printf("Running test: alignment_and_rounding...\n");
    for (size_t k = 1; k <= 33; ++k) {
        void *p = malloc(k);
        assert(p != NULL && "malloc() returned NULL unexpectedly");
        assert(((uintptr_t)p % 8) == 0 && "pointer not 8-byte aligned");

        size_t r = round8(k);
        memset(p, 0xAB, r);
        free(p);
    }
    printf("✓ alignment_and_rounding passed\n");
}

void test_non_overlapping_allocations() {
    printf("Running test: non_overlapping_allocations...\n");
    size_t a_req = 13, b_req = 29;
    size_t a_sz = round8(a_req), b_sz = round8(b_req);

    unsigned char *a = malloc(a_req);
    assert(a != NULL);
    unsigned char *b = malloc(b_req);
    assert(b != NULL);

    bool overlap = !((b >= a + a_sz) || (a >= b + b_sz));
    assert(!overlap && "Allocated payloads overlap");

    free(a);
    free(b);
    printf("✓ non_overlapping_allocations passed\n");
}

void test_exhaustion_returns_null_without_crashing() {
    printf("Running test: exhaustion_returns_null_without_crashing...\n");
    enum { MEMSIZE = 4096, OBJECTS = 64 };
    const size_t objsize = MEMSIZE / OBJECTS - HEADERSIZE;

    void *ptrs[OBJECTS + 8] = {0};
    size_t i = 0;
    for (; i < OBJECTS + 8; ++i) {
        ptrs[i] = malloc(objsize);
        if (ptrs[i] == NULL) break;
        memset(ptrs[i], (int)i & 0xFF, objsize);
    }

    assert(i == OBJECTS && "Expected exactly 64 objects to fit");
    void *extra = malloc(objsize);
    assert(extra == NULL && "Expected allocation to fail when out of memory");

    for (size_t j = 0; j < i; ++j)
        free(ptrs[j]);

    printf("✓ exhaustion_returns_null_without_crashing passed\n");
}

void test_malloc_zero_policy_is_consistent() {
    printf("Running test: malloc_zero_policy_is_consistent...\n");
    void *p = malloc(0);
    if (p) {
        assert(((uintptr_t)p % 8) == 0 && "malloc(0) returned misaligned pointer");
        free(p);
    }
    printf("✓ malloc_zero_policy_is_consistent passed\n");
}

int main(void) {
    test_alignment_and_rounding();
    test_non_overlapping_allocations();
    test_exhaustion_returns_null_without_crashing();
    test_malloc_zero_policy_is_consistent();

    printf("\nAll tests passed successfully.\n");
    return 0;
}
