#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
#define HEADER_SIZE 8
#define MIN_CHUNK_SIZE 16

// Global heap 
static union {
    char bytes[MEMLENGTH];
    double not_used;  
} heap;

static int initialized = 0;

// Chunk header
typedef struct {
    size_t size_and_flag;  
} chunk_t;

// Forward declarations
static void init_heap(void);
static chunk_t* find_free(size_t size);
static void split_chunk(chunk_t* chunk, size_t size);
static void coalesce(void);
static int valid_ptr(void* ptr);
static void check_leaks(void);

// Initialize heap
static void init_heap(void) {
    chunk_t* first = (chunk_t*)heap.bytes;
    size_t avail = MEMLENGTH - HEADER_SIZE;
    first->size_and_flag = avail & ~1; // clear LSB
    initialized = 1;
    atexit(check_leaks);
}

// Find free chunk big enough
static chunk_t* find_free(size_t size) {
    char* curr = heap.bytes;
    char* end = heap.bytes + MEMLENGTH;
    
    while (curr < end) {
        chunk_t* chunk = (chunk_t*)curr;
        size_t chunk_size = chunk->size_and_flag & ~1;
        
        if (chunk_size == 0 || curr + HEADER_SIZE + chunk_size > end) {
            break;
        }
        
        // check if free and big enough
        if (!(chunk->size_and_flag & 1) && chunk_size >= size) {
            return chunk;
        }
        
        curr += HEADER_SIZE + chunk_size;
    }
    
    return NULL;
}

// Split chunk if too big
static void split_chunk(chunk_t* chunk, size_t size) {
    size_t chunk_size = chunk->size_and_flag & ~1;
    
    if (chunk_size >= size + MIN_CHUNK_SIZE) {
        char* new_addr = (char*)chunk + HEADER_SIZE + size;
        chunk_t* new_chunk = (chunk_t*)new_addr;
        
        size_t remaining = chunk_size - size - HEADER_SIZE;
        new_chunk->size_and_flag = remaining & ~1; // free
        
        chunk->size_and_flag = (size & ~1) | (chunk->size_and_flag & 1);
    }
}

// Merge adjacent free chunks
static void coalesce(void) {
    char* curr = heap.bytes;
    char* end = heap.bytes + MEMLENGTH;
    
    while (curr < end) {
        chunk_t* chunk = (chunk_t*)curr;
        size_t chunk_size = chunk->size_and_flag & ~1;
        
        if (chunk_size == 0) break;
        
        // if current chunk is free
        if (!(chunk->size_and_flag & 1)) {
            char* next_addr = curr + HEADER_SIZE + chunk_size;
            
            if (next_addr < end) {
                chunk_t* next = (chunk_t*)next_addr;
                size_t next_size = next->size_and_flag & ~1;
                
                // if next chunk is also free, merge them
                if (next_size > 0 && !(next->size_and_flag & 1)) {
                    size_t merged = chunk_size + HEADER_SIZE + next_size;
                    chunk->size_and_flag = merged & ~1;
                    continue; // don't advance, check for more merges
                }
            }
        }
        
        curr += HEADER_SIZE + chunk_size;
    }
}

// Check if pointer is valid
static int valid_ptr(void* ptr) {
    char* p = (char*)ptr;
    
    if (p < heap.bytes || p >= heap.bytes + MEMLENGTH) {
        return 0;
    }
    
    // check alignment
    if ((uintptr_t)p % 8 != 0) {
        return 0;
    }
    
    return 1;
}

// Check for leaks at exit
static void check_leaks(void) {
    int count = 0;
    size_t bytes = 0;
    
    char* curr = heap.bytes;
    char* end = heap.bytes + MEMLENGTH;
    
    while (curr < end) {
        chunk_t* chunk = (chunk_t*)curr;
        size_t chunk_size = chunk->size_and_flag & ~1;
        
        if (chunk_size == 0) break;
        
        if (chunk->size_and_flag & 1) { // allocated
            count++;
            bytes += chunk_size;
        }
        
        curr += HEADER_SIZE + chunk_size;
    }
    
    if (count > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", 
                bytes, count);
    }
}

void* mymalloc(size_t size, char* file, int line) {
    if (!initialized) {
        init_heap();
    }
    
    if (size == 0) {
        return NULL;
    }
    
    // round up to multiple of 8
    size_t aligned = (size + 7) & ~7;
    
    chunk_t* chunk = find_free(aligned);
    
    if (!chunk) {
        coalesce(); // try merging free chunks
        chunk = find_free(aligned);
        
        if (!chunk) {
            fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", 
                    size, file, line);
            return NULL;
        }
    }
    
    split_chunk(chunk, aligned);
    
    chunk->size_and_flag |= 1; // mark allocated
    
    return (char*)chunk + HEADER_SIZE;
}

void myfree(void* ptr, char* file, int line) {
    if (ptr == NULL) {
        return;
    }
    
    if (!valid_ptr(ptr)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    chunk_t* chunk = (chunk_t*)((char*)ptr - HEADER_SIZE);
    
    if (!valid_ptr(chunk)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    // check if already free
    if (!(chunk->size_and_flag & 1)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    // check size is reasonable
    size_t chunk_size = chunk->size_and_flag & ~1;
    if (chunk_size == 0 || (char*)chunk + HEADER_SIZE + chunk_size > heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    chunk->size_and_flag &= ~1; // mark free
    
    coalesce(); // merge adjacent free chunks
}