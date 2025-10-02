/**
 * CS 214: Systems Programming, Fall 2025
 * Project 1: My Little Malloc
 *
 * mymalloc.c - A custom memory allocator implementation.
 *
 * This library provides a custom version of malloc() and free() that manages a
 * fixed-size heap. It includes error detection for common memory issues like
 * freeing invalid pointers, double-freeing, and reports memory leaks upon exit.
 *
 * The heap is managed as a contiguous block of memory, partitioned into chunks.
 * Each chunk has a metadata header followed by a payload for user data.
 * A first-fit algorithm is used for allocation, and adjacent free blocks
 * are coalesced upon deallocation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

// Define the total size of the memory pool (heap).
#define MEMLENGTH 4096
#define HEADER_SIZE sizeof(MetaData)

// The heap is a static global array, aligned to 8 bytes using a union.
static union {
    char bytes[MEMLENGTH];
    double alignment_dummy; // Ensures the union is 8-byte aligned.
} heap;

// A single static flag to track if the heap has been initialized.
static int is_initialized = 0;

// Metadata structure to precede each chunk of memory.
// It is crucial that the size of this struct is a multiple of 8 to maintain alignment.
typedef struct MetaData {
    size_t size;     // Size of the payload (the usable memory for the client).
    int is_free;     // Flag: 1 if the chunk is free, 0 if it is in use.
    struct MetaData *next; // Pointer to the next chunk's metadata header.
} MetaData;


/**
 * @brief Reports memory leaks by traversing the heap at program exit.
 *
 * This function is registered with atexit() during initialization. It scans
 * all memory chunks and reports the total number and size of any chunks
 * that were allocated but never freed.
 */
void report_leaks() {
    MetaData *current = (MetaData *)heap.bytes;
    int leaked_chunks = 0;
    size_t leaked_bytes = 0;

    // Traverse the entire heap from the beginning.
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

/**
 * @brief Initializes the heap for the memory allocator.
 *
 * This function is called only once, on the first invocation of mymalloc() or
 * myfree(). It sets up the entire heap as a single, large, free chunk and
 * registers the leak detection function to run at program exit.
 */
void initialize_heap() {
    if (is_initialized) {
        return;
    }

    // The first chunk's header starts at the beginning of the heap.
    MetaData *first_chunk = (MetaData *)heap.bytes;
    first_chunk->size = MEMLENGTH - HEADER_SIZE;
    first_chunk->is_free = 1;
    first_chunk->next = NULL; // Initially, there is only one chunk.

    // Register the leak detector to be called automatically on exit.
    atexit(report_leaks);

    is_initialized = 1;
}

/**
 * @brief Rounds a given size up to the nearest multiple of 8.
 *
 * This is necessary to maintain 8-byte alignment for all allocations.
 * @param size The original size.
 * @return The size rounded up to the nearest multiple of 8.
 */
static size_t align_to_8(size_t size) {
    return (size + 7) & ~7;
}

/**
 * @brief Allocates a block of memory from the heap.
 *
 * Implements a first-fit algorithm. It searches for the first free chunk
 * that is large enough to hold the requested size. If the chunk is larger
 * than necessary, it may be split.
 *
 * @param requested_size The number of bytes requested by the client.
 * @param file The source file where the malloc call originated.
 * @param line The line number of the malloc call.
 * @return A pointer to the allocated payload, or NULL if allocation fails.
 */
void *mymalloc(size_t requested_size, char *file, int line) {
    initialize_heap();

    // Align the requested size to ensure 8-byte alignment.
    size_t required_payload_size = align_to_8(requested_size);

    MetaData *current = (MetaData *)heap.bytes;

    // First-fit search: find the first available and suitable chunk.
    while (current != NULL) {
        if (current->is_free && current->size >= required_payload_size) {
            // Found a suitable chunk. Now, decide whether to split it.
            size_t remaining_size = current->size - required_payload_size;

            // A split is only worthwhile if the remaining space can hold a header
            // and a minimal payload (at least 8 bytes).
            if (remaining_size >= HEADER_SIZE + 8) {
                MetaData *new_free_chunk = (MetaData *)((char *)current + HEADER_SIZE + required_payload_size);
                new_free_chunk->size = remaining_size - HEADER_SIZE;
                new_free_chunk->is_free = 1;
                new_free_chunk->next = current->next;

                current->size = required_payload_size;
                current->next = new_free_chunk;
            }
            
            // Mark the chunk as used and return a pointer to its payload.
            current->is_free = 0;
            return (void *)((char *)current + HEADER_SIZE);
        }
        current = current->next;
    }

    // If we reach here, no suitable chunk was found.
    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", requested_size, file, line);
    return NULL;
}

/**
 * @brief Coalesces adjacent free memory chunks.
 *
 * Traverses the heap and merges any two consecutive free chunks into a
 * single larger free chunk to reduce fragmentation.
 */
void coalesce_chunks() {
    MetaData *current = (MetaData *)heap.bytes;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            // The next chunk is also free, so merge them.
            current->size += HEADER_SIZE + current->next->size;
            current->next = current->next->next;
            // Stay on the current chunk to check if the new, larger chunk
            // can be merged with the one that follows it.
            continue;
        }
        current = current->next;
    }
}

/**
 * @brief Deallocates a previously allocated block of memory.
 *
 * Validates the provided pointer to ensure it's a valid address obtained
 * from mymalloc. If valid, it marks the chunk as free and attempts to
 * coalesce it with adjacent free chunks.
 *
 * @param ptr The pointer to the memory block to free.
 * @param file The source file where the free call originated.
 * @param line The line number of the free call.
 */
void myfree(void *ptr, char *file, int line) {
    initialize_heap();

    if (ptr == NULL) {
        return; // Freeing a null pointer is a no-op.
    }

    // Pointer validation.
    MetaData *current = (MetaData *)heap.bytes;
    MetaData *target_header = NULL;
    int is_valid_pointer = 0;

    // Check if the pointer points to the start of a payload we manage.
    while (current != NULL) {
        void *payload_start = (void *)((char *)current + HEADER_SIZE);
        if (ptr == payload_start) {
            target_header = current;
            is_valid_pointer = 1;
            break;
        }
        current = current->next;
    }

    if (!is_valid_pointer) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    // Check for double-free.
    if (target_header->is_free) {
        fprintf(stderr, "free: Inappropriate pointer (double free) (%s:%d)\n", file, line);
        exit(2);
    }

    // Mark the chunk as free.
    target_header->is_free = 1;

    // Coalesce adjacent free chunks.
    coalesce_chunks();
}
