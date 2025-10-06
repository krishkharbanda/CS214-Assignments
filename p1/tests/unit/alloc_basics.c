#include <criterion/criterion.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.h"  

static inline size_t round8(size_t k) { return (k + 7) & ~((size_t)7); }

Test(alloc_basics, alignment_and_rounding) {
    for (size_t k = 1; k <= 33; ++k) {
        void *p = malloc(k);
        cr_assert_not_null(p, "malloc(%zu) returned NULL unexpectedly", k);
        cr_assert(((uintptr_t)p % 8) == 0, "pointer not 8-byte aligned for k=%zu", k);

        // Write exactly the rounded payload (should be safe for allocator).
        size_t r = round8(k);
        memset(p, 0xAB, r);
        free(p);
    }
}

Test(alloc_basics, non_overlapping_allocations) {
    size_t a_req = 13, b_req = 29;
    size_t a_sz = round8(a_req), b_sz = round8(b_req);

    unsigned char *a = malloc(a_req);
    cr_assert_not_null(a);
    unsigned char *b = malloc(b_req);
    cr_assert_not_null(b);

    // Ranges [a, a+a_sz) and [b, b+b_sz) must not overlap
    bool overlap = !((b >= a + a_sz) || (a >= b + b_sz));
    cr_assert_not(overlap, "Allocated payloads overlap");

    free(a);
    free(b);
}

Test(alloc_basics, exhaustion_returns_null_without_crashing) {
    enum { MEMSIZE = 4096, OBJECTS = 64 };
    const size_t objsize = MEMSIZE / OBJECTS - HEADERSIZE;

    void *ptrs[OBJECTS + 8] = {0};
    size_t i = 0;
    for (; i < OBJECTS + 8; ++i) {
        ptrs[i] = malloc(objsize);
        if (ptrs[i] == NULL) break; // expected to break before i==OBJECTS
        memset(ptrs[i], (int)i & 0xFF, objsize);
    }
    cr_assert_eq(i, OBJECTS,
                "Expected exactly %d objects to fit, but got %zu", OBJECTS, i);

    void *extra = malloc(objsize);
    cr_assert_null(extra, "Expected the 65th allocation to fail");

    for (size_t j = 0; j < i; ++j) {
        free(ptrs[j]);
    }
}

Test(alloc_basics, malloc_zero_policy_is_consistent) {
    void *p = malloc(0);
    if (p) {
        cr_assert(((uintptr_t)p % 8) == 0, "malloc(0) returned misaligned pointer");
        free(p); 
    }
}
