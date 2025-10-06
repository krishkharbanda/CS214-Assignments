#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mymalloc.h"

void test_task1_immediate_free_120_iter(void) {
    printf("Running memgrind::task1_immediate_free_120_iter...\n");

    for (int i = 0; i < 120; ++i) {
        void *p = malloc(1);
        assert(p != NULL && "malloc returned NULL unexpectedly");
        memset(p, 0, 1);
        free(p);
    }

    printf("✓ passed\n");
}

void test_task2_batch_120_then_free(void) {
    printf("Running memgrind::task2_batch_120_then_free...\n");

    void *ptrs[120] = {0};
    for (int i = 0; i < 120; ++i) {
        ptrs[i] = malloc(1);
        assert(ptrs[i] != NULL && "malloc failed during batch alloc");
    }
    for (int i = 0; i < 120; ++i)
        free(ptrs[i]);

    printf("✓ passed\n");
}

void test_task3_random_alloc_free_until_120_allocs(void) {
    printf("Running memgrind::task3_random_alloc_free_until_120_allocs...\n");

    void *ptrs[120] = {0};
    int live = 0, total = 0;
    unsigned seed = 0xC0FFEEU;

    while (total < 120) {
        int do_alloc = (seed = 1664525U * seed + 1013904223U) >> 31; // cheap PRNG
        if (do_alloc || live == 0) {
            // allocate 1 byte
            for (int i = 0; i < 120; ++i) {
                if (!ptrs[i]) {
                    ptrs[i] = malloc(1);
                    assert(ptrs[i] != NULL && "malloc failed during random alloc");
                    ++live;
                    ++total;
                    break;
                }
            }
        } else {
            // free a random live slot
            int idx = (int)((seed >> 8) % 120);
            for (int k = 0; k < 120; ++k) {
                int j = (idx + k) % 120;
                if (ptrs[j]) {
                    free(ptrs[j]);
                    ptrs[j] = NULL;
                    --live;
                    break;
                }
            }
        }
    }

    // free any remaining allocations
    for (int i = 0; i < 120; ++i)
        if (ptrs[i])
            free(ptrs[i]);

    printf("✓ passed\n");
}

int main(void) {
    test_task1_immediate_free_120_iter();
    test_task2_batch_120_then_free();
    test_task3_random_alloc_free_until_120_allocs();

    printf("\nAll memgrind tests passed successfully.\n");
    return 0;
}
