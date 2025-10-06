#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mymalloc.h"

void test_free_null_is_noop(void) {
    printf("Running free_semantics::free_null_is_noop...\n");
    free(NULL);
    // If we got here, no crash occurred
    printf("✓ passed\n");
}

void test_allocator_does_not_modify_payload_while_allocated(void) {
    printf("Running free_semantics::allocator_does_not_modify_payload_while_allocated...\n");

    const size_t n = 64;
    unsigned char *p = malloc(n);
    assert(p != NULL && "malloc failed");

    for (size_t i = 0; i < n; ++i)
        p[i] = (unsigned char)(i ^ 0x5A);

    // Perform some unrelated alloc/free to exercise the allocator
    void *q = malloc(32);
    assert(q != NULL && "second malloc failed");
    free(q);

    // Ensure our payload stayed intact
    for (size_t i = 0; i < n; ++i)
        assert(p[i] == (unsigned char)(i ^ 0x5A) && "payload modified during allocation/free");

    free(p);
    printf("✓ passed\n");
}

int main(void) {
    test_free_null_is_noop();
    test_allocator_does_not_modify_payload_while_allocated();

    printf("\nAll free_semantics tests passed successfully.\n");
    return 0;
}
