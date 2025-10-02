// tests/stress/memgrind.c
#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

Test(memgrind, task1_immediate_free_120_iter) {
    for (int i = 0; i < 120; ++i) {
        void *p = malloc(1);
        cr_assert_not_null(p);
        memset(p, 0, 1);
        free(p);
    }
}

Test(memgrind, task2_batch_120_then_free) {
    void *ptrs[120] = {0};
    for (int i = 0; i < 120; ++i) {
        ptrs[i] = malloc(1);
        cr_assert_not_null(ptrs[i], "alloc %d failed", i);
    }
    for (int i = 0; i < 120; ++i) free(ptrs[i]);
}

Test(memgrind, task3_random_alloc_free_until_120_allocs) {
    void *ptrs[120] = {0};
    int live = 0, total = 0;
    unsigned seed = 0xC0FFEEU;

    while (total < 120) {
        int do_alloc = (seed = 1664525U * seed + 1013904223U) >> 31; // cheap PRNG
        if (do_alloc || live == 0) {
            // allocate 1 byte
            for (int i = 0; i < 120; ++i) if (!ptrs[i]) {
                ptrs[i] = malloc(1);
                cr_assert_not_null(ptrs[i], "alloc %d failed", total);
                ++live; ++total;
                break;
            }
        } else {
            // free a random live slot
            int idx = (int)((seed >> 8) % 120);
            for (int k = 0; k < 120; ++k) {
                int j = (idx + k) % 120;
                if (ptrs[j]) { free(ptrs[j]); ptrs[j] = NULL; --live; break; }
            }
        }
    }
    // free any remaining
    for (int i = 0; i < 120; ++i) if (ptrs[i]) free(ptrs[i]);
}
