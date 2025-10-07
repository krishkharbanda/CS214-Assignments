#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "mymalloc.h"

#define NUM_RUNS 50
#define ALLOC_COUNT 120

// Get current time
static double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000L + tv.tv_usec;
}

// Task 1: malloc and free 1-byte objects 120 times
static void task1() {
    for (int i = 0; i < ALLOC_COUNT; i++) {
        char* ptr = malloc(1);
        if (ptr == NULL) {
            printf("Task 1 failed at %d\n", i);
            exit(1);
        }
        free(ptr);
    }
}

// Task 2: allocate 120 objects then free them all
static void task2() {
    char* ptrs[ALLOC_COUNT];
    
    // allocate
    for (int i = 0; i < ALLOC_COUNT; i++) {
        ptrs[i] = malloc(1);
        if (ptrs[i] == NULL) {
            printf("Task 2 failed at %d\n", i);
            exit(1);
        }
    }
    
    // free
    for (int i = 0; i < ALLOC_COUNT; i++) {
        free(ptrs[i]);
    }
}

// Task 3: random allocation/deallocation
static void task3() {
    char* ptrs[ALLOC_COUNT];
    int allocated = 0;
    int total = 0;
    
    for (int i = 0; i < ALLOC_COUNT; i++) {
        ptrs[i] = NULL;
    }
    
    while (total < ALLOC_COUNT) {
        int choice = rand() % 2;
        
        if (choice == 0) {
            // allocate if we have space
            if (allocated < ALLOC_COUNT) {
                for (int i = 0; i < ALLOC_COUNT; i++) {
                    if (ptrs[i] == NULL) {
                        ptrs[i] = malloc(1);
                        if (ptrs[i] == NULL) {
                            printf("Task 3 malloc failed\n");
                            exit(1);
                        }
                        allocated++;
                        total++;
                        break;
                    }
                }
            }
        } else {
            // deallocate if we have objects
            if (allocated > 0) {
                int start = rand() % ALLOC_COUNT;
                for (int i = 0; i < ALLOC_COUNT; i++) {
                    int idx = (start + i) % ALLOC_COUNT;
                    if (ptrs[idx] != NULL) {
                        free(ptrs[idx]);
                        ptrs[idx] = NULL;
                        allocated--;
                        break;
                    }
                }
            }
        }
    }
    
    // clean up remaining
    for (int i = 0; i < ALLOC_COUNT; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
}

// Task 4: linked list operations
static void task4() {
    typedef struct node {
        int data;
        struct node* next;
    } node_t;
    
    node_t* head = NULL;
    
    // create list with 60 nodes
    for (int i = 0; i < 60; i++) {
        node_t* new_node = malloc(sizeof(node_t));
        if (new_node == NULL) {
            printf("Task 4 malloc failed at %d\n", i);
            exit(1);
        }
        new_node->data = i;
        new_node->next = head;
        head = new_node;
    }
    
    // remove every other node
    node_t* curr = head;
    node_t* prev = NULL;
    int cnt = 0;
    
    while (curr != NULL) {
        if (cnt % 2 == 1) {
            if (prev != NULL) {
                prev->next = curr->next;
            } else {
                head = curr->next;
            }
            node_t* temp = curr;
            curr = curr->next;
            free(temp);
        } else {
            prev = curr;
            curr = curr->next;
        }
        cnt++;
    }
    
    // free remaining
    while (head != NULL) {
        node_t* temp = head;
        head = head->next;
        free(temp);
    }
}

// Task 5: dynamic array resizing
static void task5() {
    int size = 10;
    int max_ops = 100;
    
    int* array = malloc(size * sizeof(int));
    if (array == NULL) {
        printf("Task 5 initial malloc failed\n");
        exit(1);
    }
    
    int curr_size = size;
    
    // fill initial array
    for (int i = 0; i < size; i++) {
        array[i] = i;
    }
    
    // do resize operations
    for (int op = 0; op < max_ops; op++) {
        int operation = rand() % 3;
        
        if (operation == 0 && curr_size > 5) {
            // shrink
            int new_size = curr_size / 2;
            int* new_array = malloc(new_size * sizeof(int));
            if (new_array == NULL) {
                printf("Task 5 shrink failed\n");
                exit(1);
            }
            
            for (int i = 0; i < new_size; i++) {
                new_array[i] = array[i];
            }
            
            free(array);
            array = new_array;
            curr_size = new_size;
        } else if (operation == 1 && curr_size < 80) {
            // grow
            int new_size = curr_size * 2;
            int* new_array = malloc(new_size * sizeof(int));
            if (new_array == NULL) {
                printf("Task 5 grow failed\n");
                exit(1);
            }
            
            for (int i = 0; i < curr_size; i++) {
                new_array[i] = array[i];
            }
            
            for (int i = curr_size; i < new_size; i++) {
                new_array[i] = i;
            }
            
            free(array);
            array = new_array;
            curr_size = new_size;
        }
    }
    
    free(array);
}

int main() {
    printf("Starting stress test with %d runs\n", NUM_RUNS);
    
    void (*tasks[])(void) = {task1, task2, task3, task4, task5};
    const char* names[] = {
        "Task 1: malloc/free cycles",
        "Task 2: bulk alloc/free", 
        "Task 3: random ops",
        "Task 4: linked list",
        "Task 5: dynamic arrays"
    };
    
    srand(time(NULL));
    long total = 0; // not sure if this is needed but someone said to include it
    for (int i = 0; i < 5; i++) {
        printf("\n%s:\n", names[i]);
        
        double start = get_time();
        
        for (int run = 0; run < NUM_RUNS; run++) {
            tasks[i]();
        }
        
        double end = get_time();
        double avg = (end - start) / NUM_RUNS;
        total += avg; 
        printf("Average time: %.3f microseconds\n", avg);
    }
    printf("\nAverage workload time per run: %ld microseconds\n", total / 5);
    printf("\nMemgrind done!\n");
    return 0;
}