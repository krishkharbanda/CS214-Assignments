#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
#define HEADER_SIZE sizeof(Metadata)

static union {
    char bytes[4096];
    double alignment_dummy; 
} heap;

static int is_init = 0;

typedef struct Metadata {
    size_t size;
    int is_free;
    struct Metadata *next;
} Metadata;

void find_leaks() {
    Metadata *current = (Metadata *)heap.bytes;
    int leaked_chunks = 0;
    size_t leaked_bytes = 0;

    while (current != NULL) {
        if (!current->is_free) {
            leaked_chunks++;
            leaked_bytes += current->size;
        }
        current = current->next;
    }

    if (leaked_chunks > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", leaked_bytes, leaked_chunks);
    }
}

void init_heap() {
    if (is_init) {
        return;
    }

    Metadata *first_chunk = (Metadata *)heap.bytes;
    first_chunk->size = 4096 - HEADER_SIZE;
    first_chunk->is_free = 1;
    first_chunk->next = NULL;

    atexit(find_leaks);

    is_init = 1;
}

void *mymalloc(size_t requested_size, char *file, int line) {
    init_heap();

    size_t required_size = (requested_size + 7) & ~7;

    Metadata *current = (Metadata *)heap.bytes;

    while (current != NULL) {
        if (current->is_free && current->size >= required_size) {
            size_t remaining_size = current->size - required_size;
            if (remaining_size >= HEADER_SIZE + 8) {
                Metadata *new_chunk = (Metadata *)((char *)current + HEADER_SIZE + required_size);
                new_chunk->size = remaining_size - HEADER_SIZE;
                new_chunk->is_free = 1;
                new_chunk->next = current->next;

                current->size = required_size;
                current->next = new_chunk;
            }
            
            current->is_free = 0;
            return (void *)((char *)current + HEADER_SIZE);
        }
        current = current->next;
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", requested_size, file, line);
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    init_heap();

    if (ptr == NULL) {
        return;
    }

    Metadata *current = (Metadata *)heap.bytes;
    Metadata *target = NULL;
    int is_valid = 0;

    while (current != NULL) {
        void *start = (void *)((char *)current + HEADER_SIZE);
        if (ptr == start) {
            target = current;
            is_valid = 1;
            break;
        }
        current = current->next;
    }

    if (!is_valid) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    if (target->is_free) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    target->is_free = 1;

    current = (Metadata *)heap.bytes;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += HEADER_SIZE + current->next->size;
            current->next = current->next->next;
            continue;
        }
        current = current->next;
    }
}
