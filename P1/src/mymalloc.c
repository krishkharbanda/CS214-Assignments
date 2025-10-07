#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
#define HEADER_SIZE 8
#define MIN_CHUNK_SIZE 16
#define ALLOCATED 1
#define FREE 0

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

static int heap_initialized = 0;

typedef struct {
    size_t size_and_flag;
} chunk_header_t;

#define GET_SIZE(chunk) ((chunk)->size_and_flag & ~1)
#define IS_ALLOCATED(chunk) ((chunk)->size_and_flag & 1)
#define SET_ALLOCATED(chunk) ((chunk)->size_and_flag |= 1)
#define SET_FREE(chunk) ((chunk)->size_and_flag &= ~1)
#define SET_SIZE(chunk, size) ((chunk)->size_and_flag = ((size) & ~1) | ((chunk)->size_and_flag & 1))

static void init_heap(void);
static chunk_header_t* find_free_chunk(size_t needed_size);
static void split_chunk(chunk_header_t* chunk, size_t needed_size);
static void coalesce_free_chunks(void);
static int is_valid_heap_pointer(void* ptr);
static void leak_detector(void);

static void init_heap(void) {
    chunk_header_t* first_chunk = (chunk_header_t*)heap.bytes;
    
    size_t available_size = MEMLENGTH - HEADER_SIZE;
    SET_SIZE(first_chunk, available_size);
    SET_FREE(first_chunk);
    
    heap_initialized = 1;
    
    atexit(leak_detector);
}

static chunk_header_t* find_free_chunk(size_t needed_size) {
    char* current = heap.bytes;
    char* heap_end = heap.bytes + MEMLENGTH;
    
    while (current < heap_end) {
        chunk_header_t* chunk = (chunk_header_t*)current;
        size_t chunk_size = GET_SIZE(chunk);
        
        if (chunk_size == 0 || current + HEADER_SIZE + chunk_size > heap_end) {
            break;
        }
        
        if (!IS_ALLOCATED(chunk) && chunk_size >= needed_size) {
            return chunk;
        }
        
        current += HEADER_SIZE + chunk_size;
    }
    
    return NULL;
}

static void split_chunk(chunk_header_t* chunk, size_t needed_size) {
    size_t chunk_size = GET_SIZE(chunk);
    
    if (chunk_size >= needed_size + MIN_CHUNK_SIZE) {
        char* new_chunk_addr = (char*)chunk + HEADER_SIZE + needed_size;
        chunk_header_t* new_chunk = (chunk_header_t*)new_chunk_addr;
        
        size_t remaining_size = chunk_size - needed_size - HEADER_SIZE;
        SET_SIZE(new_chunk, remaining_size);
        SET_FREE(new_chunk);
        
        SET_SIZE(chunk, needed_size);
    }
}

static void coalesce_free_chunks(void) {
    char* current = heap.bytes;
    char* heap_end = heap.bytes + MEMLENGTH;
    
    while (current < heap_end) {
        chunk_header_t* chunk = (chunk_header_t*)current;
        size_t chunk_size = GET_SIZE(chunk);
        
        if (chunk_size == 0) break;
        
        if (!IS_ALLOCATED(chunk)) {
            char* next_addr = current + HEADER_SIZE + chunk_size;
            
            if (next_addr < heap_end) {
                chunk_header_t* next_chunk = (chunk_header_t*)next_addr;
                size_t next_size = GET_SIZE(next_chunk);
                
                if (next_size > 0 && !IS_ALLOCATED(next_chunk)) {
                    size_t merged_size = chunk_size + HEADER_SIZE + next_size;
                    SET_SIZE(chunk, merged_size);
                    continue;
                }
            }
        }
        
        current += HEADER_SIZE + chunk_size;
    }
}

static int is_valid_heap_pointer(void* ptr) {
    char* char_ptr = (char*)ptr;
    
    if (char_ptr < heap.bytes || char_ptr >= heap.bytes + MEMLENGTH) {
        return 0;
    }
    
    if ((uintptr_t)char_ptr % 8 != 0) {
        return 0;
    }
    
    return 1;
}

static void leak_detector(void) {
    int leak_count = 0;
    size_t leak_bytes = 0;
    
    char* current = heap.bytes;
    char* heap_end = heap.bytes + MEMLENGTH;
    
    while (current < heap_end) {
        chunk_header_t* chunk = (chunk_header_t*)current;
        size_t chunk_size = GET_SIZE(chunk);
        
        if (chunk_size == 0) break;
        
        if (IS_ALLOCATED(chunk)) {
            leak_count++;
            leak_bytes += chunk_size;
        }
        
        current += HEADER_SIZE + chunk_size;
    }
    
    if (leak_count > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", 
                leak_bytes, leak_count);
    }
}

void* mymalloc(size_t size, char* file, int line) {
    if (!heap_initialized) {
        init_heap();
    }
    
    if (size == 0) {
        return NULL;
    }
    
    size_t aligned_size = (size + 7) & ~7;
    
    chunk_header_t* chunk = find_free_chunk(aligned_size);
    
    if (!chunk) {
        coalesce_free_chunks();
        chunk = find_free_chunk(aligned_size);
        
        if (!chunk) {
            fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", 
                    size, file, line);
            return NULL;
        }
    }
    
    split_chunk(chunk, aligned_size);
    
    SET_ALLOCATED(chunk);
    
    return (char*)chunk + HEADER_SIZE;
}

void myfree(void* ptr, char* file, int line) {
    if (ptr == NULL) {
        return;
    }
    
    if (!is_valid_heap_pointer(ptr)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    chunk_header_t* chunk = (chunk_header_t*)((char*)ptr - HEADER_SIZE);
    
    if (!is_valid_heap_pointer(chunk)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    if (!IS_ALLOCATED(chunk)) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    size_t chunk_size = GET_SIZE(chunk);
    if (chunk_size == 0 || (char*)chunk + HEADER_SIZE + chunk_size > heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
    
    SET_FREE(chunk);
    
    coalesce_free_chunks();
}