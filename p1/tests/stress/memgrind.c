#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mymalloc.h"

// Small repeated allocations and immediate frees
void test_small_alloc_free_pattern(void) {
    printf("Running test_small_alloc_free_pattern...\n");

    for (int i = 0; i < 120; ++i) {
        void *block = malloc(1);
        assert(block != NULL && "malloc returned NULL unexpectedly");
        memset(block, 0, 1);
        free(block);
    }

    printf("✓ passed\n");
}

// Allocate many small blocks, then free all at once
void test_batch_alloc_then_release(void) {
    printf("Running test_batch_alloc_then_release...\n");

    void *blocks[120] = {0};

    for (int i = 0; i < 120; ++i) {
        blocks[i] = malloc(1);
        assert(blocks[i] != NULL && "malloc failed during batch alloc");
    }

    for (int i = 0; i < 120; ++i)
        free(blocks[i]);

    printf("✓ passed\n");
}

// Random allocation and freeing pattern
void test_random_alloc_free_pattern(void) {
    printf("Running test_random_alloc_free_pattern...\n");

    void *active_blocks[120] = {0};
    int active_count = 0, total_allocs = 0;
    unsigned seed = 0xC0FFEEU; // simple deterministic PRNG seed

    while (total_allocs < 120) {
        int should_allocate = (seed = 1664525U * seed + 1013904223U) >> 31;

        if (should_allocate || active_count == 0) {
            for (int i = 0; i < 120; ++i) {
                if (!active_blocks[i]) {
                    active_blocks[i] = malloc(1);
                    assert(active_blocks[i] != NULL && "malloc failed during random alloc");
                    ++active_count;
                    ++total_allocs;
                    break;
                }
            }
        } else {
            int start_index = (int)((seed >> 8) % 120);
            for (int k = 0; k < 120; ++k) {
                int idx = (start_index + k) % 120;
                if (active_blocks[idx]) {
                    free(active_blocks[idx]);
                    active_blocks[idx] = NULL;
                    --active_count;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < 120; ++i)
        if (active_blocks[i])
            free(active_blocks[i]);

    printf("✓ passed\n");
}

// Linked list allocation and traversal simulation
typedef struct ListNode {
    int value;
    struct ListNode *next;
} ListNode;

void test_linked_list_usage(void) {
    printf("Running test_linked_list_usage...\n");

    ListNode *head = NULL;

    // Build list with 100 nodes
    for (int i = 0; i < 100; ++i) {
        ListNode *node = (ListNode *)malloc(sizeof(ListNode));
        assert(node != NULL && "malloc failed while building linked list");
        node->value = i;
        node->next = head;
        head = node;
    }

    // Traverse to check consistency
    int count = 0;
    for (ListNode *curr = head; curr != NULL; curr = curr->next)
        count++;
    assert(count == 100 && "Linked list traversal count mismatch");

    // Free all nodes
    while (head) {
        ListNode *temp = head;
        head = head->next;
        free(temp);
    }

    printf("✓ passed\n");
}

// Binary tree allocation and deallocation simulation
typedef struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode* create_balanced_tree(int depth) {
    if (depth == 0)
        return NULL;

    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (!node)
        return NULL;

    node->value = depth;
    node->left = create_balanced_tree(depth - 1);
    node->right = create_balanced_tree(depth - 1);
    return node;
}

void free_tree(TreeNode *root) {
    if (!root)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

void test_binary_tree_usage(void) {
    printf("Running test_binary_tree_usage...\n");

    TreeNode *root = create_balanced_tree(6); // 63 nodes total
    assert(root != NULL && "Tree root allocation failed");

    free_tree(root);

    printf("✓ passed\n");
}

int main(void) {
    test_small_alloc_free_pattern();
    test_batch_alloc_then_release();
    test_random_alloc_free_pattern();
    test_linked_list_usage();
    test_binary_tree_usage();

    printf("\nAll memgrind tests passed successfully.\n");
    return 0;
}
