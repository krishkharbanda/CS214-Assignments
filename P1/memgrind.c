#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "mymalloc.h"

#define NUM_RUNS 50
#define ALLOC_COUNT 120

static double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static void task1() {
    for (int i = 0; i < ALLOC_COUNT; i++) {
        char* ptr = malloc(1);
        if (ptr == NULL) {
            printf("Task 1: malloc failed at iteration %d\n", i);
            exit(1);
        }
        free(ptr);
    }
}

static void task2() {
    char* ptrs[ALLOC_COUNT];
    
    for (int i = 0; i < ALLOC_COUNT; i++) {
        ptrs[i] = malloc(1);
        if (ptrs[i] == NULL) {
            printf("Task 2: malloc failed at iteration %d\n", i);
            exit(1);
        }
    }
    
    for (int i = 0; i < ALLOC_COUNT; i++) {
        free(ptrs[i]);
    }
}

static void task3() {
    char* ptrs[ALLOC_COUNT];
    int allocated_count = 0;
    int total_allocated = 0;
    
    for (int i = 0; i < ALLOC_COUNT; i++) {
        ptrs[i] = NULL;
    }
    
    while (total_allocated < ALLOC_COUNT) {
        int choice = rand() % 2;
        
        if (choice == 0) {
            if (allocated_count < ALLOC_COUNT) {
                for (int i = 0; i < ALLOC_COUNT; i++) {
                    if (ptrs[i] == NULL) {
                        ptrs[i] = malloc(1);
                        if (ptrs[i] == NULL) {
                            printf("Task 3: malloc failed\n");
                            exit(1);
                        }
                        allocated_count++;
                        total_allocated++;
                        break;
                    }
                }
            }
        } else {
            if (allocated_count > 0) {
                int start = rand() % ALLOC_COUNT;
                for (int i = 0; i < ALLOC_COUNT; i++) {
                    int index = (start + i) % ALLOC_COUNT;
                    if (ptrs[index] != NULL) {
                        free(ptrs[index]);
                        ptrs[index] = NULL;
                        allocated_count--;
                        break;
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < ALLOC_COUNT; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
}

static void task4() {
    typedef struct node {
        int data;
        struct node* next;
    } node_t;
    
    node_t* head = NULL;
    
    for (int i = 0; i < 60; i++) {
        node_t* new_node = malloc(sizeof(node_t));
        if (new_node == NULL) {
            printf("Task 4: malloc failed for node %d\n", i);
            exit(1);
        }
        new_node->data = i;
        new_node->next = head;
        head = new_node;
    }
    
    node_t* current = head;
    node_t* prev = NULL;
    int count = 0;
    
    while (current != NULL) {
        if (count % 2 == 1) {
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            node_t* to_delete = current;
            current = current->next;
            free(to_delete);
        } else {
            prev = current;
            current = current->next;
        }
        count++;
    }
    
    while (head != NULL) {
        node_t* temp = head;
        head = head->next;
        free(temp);
    }
}

static void task5() {
    const int initial_size = 10;
    const int max_operations = 100;
    
    int* array = malloc(initial_size * sizeof(int));
    if (array == NULL) {
        printf("Task 5: Initial malloc failed\n");
        exit(1);
    }
    
    int current_size = initial_size;
    
    for (int i = 0; i < initial_size; i++) {
        array[i] = i;
    }
    
    for (int op = 0; op < max_operations; op++) {
        int operation = rand() % 3;
        
        if (operation == 0 && current_size > 5) {
            int new_size = current_size / 2;
            int* new_array = malloc(new_size * sizeof(int));
            if (new_array == NULL) {
                printf("Task 5: malloc failed during shrink\n");
                exit(1);
            }
            
            for (int i = 0; i < new_size; i++) {
                new_array[i] = array[i];
            }
            
            free(array);
            array = new_array;
            current_size = new_size;
        } else if (operation == 1 && current_size < 80) {
            int new_size = current_size * 2;
            int* new_array = malloc(new_size * sizeof(int));
            if (new_array == NULL) {
                printf("Task 5: malloc failed during grow\n");
                exit(1);
            }
            
            for (int i = 0; i < current_size; i++) {
                new_array[i] = array[i];
            }
            
            for (int i = current_size; i < new_size; i++) {
                new_array[i] = i;
            }
            
            free(array);
            array = new_array;
            current_size = new_size;
        }
    }
    
    free(array);
}

int main() {
    printf("Starting memory stress test with %d runs per task...\n", NUM_RUNS);
    
    void (*tasks[])(void) = {task1, task2, task3, task4, task5};
    const char* task_names[] = {
        "Task 1: Immediate malloc/free",
        "Task 2: Bulk allocate then free",
        "Task 3: Random allocate/deallocate",
        "Task 4: Linked list operations",
        "Task 5: Dynamic array operations"
    };
    
    srand(time(NULL));
    
    for (int task_num = 0; task_num < 5; task_num++) {
        printf("\n%s:\n", task_names[task_num]);
        
        double start_time = get_time();
        
        for (int run = 0; run < NUM_RUNS; run++) {
            tasks[task_num]();
        }
        
        double end_time = get_time();
        double total_time = end_time - start_time;
        double avg_time = total_time / NUM_RUNS;
        
        printf("Average time per run: %.6f seconds\n", avg_time);
    }
    
    return 0;
}