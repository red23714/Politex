/*
  File for 'max-threads' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"

void test_max_mem_malloc(void) {
    int allocation_count = 0;
    void **pointers = NULL;
    size_t array_size = 1000;
    
    pointers = malloc(sizeof(void*) * array_size);
    if (!pointers) {
        printf("Failed to allocate pointers array\n");
        return;
    }
    
    printf("Testing maximum malloc allocations (256 bytes each)...\n");
    
    while(1) {
        if (allocation_count >= array_size) {
            // Увеличиваем массив указателей
            array_size *= 2;
            void **new_pointers = realloc(pointers, sizeof(void*) * array_size);
            if (!new_pointers) {
                printf("Failed to expand pointers array at count %d\n", allocation_count);
                break;
            }
            pointers = new_pointers;
        }
        
        pointers[allocation_count] = malloc(256);
        if (!pointers[allocation_count]) {
            printf("malloc failed at allocation %d\n", allocation_count);
            break;
        }
        
        allocation_count++;
        
        if (allocation_count % 100 == 0) {
            printf("Allocated %d blocks so far...\n", allocation_count);
        }
    }
    
    printf("Maximum malloc allocations: %d\n", allocation_count);
    
    // Освобождаем память
    for (int i = 0; i < allocation_count; i++) {
        free(pointers[i]);
    }
    free(pointers);
}

void test_max_mem_calloc(void) {
    int allocation_count = 0;
    void **pointers = NULL;
    size_t array_size = 1000;
    
    pointers = malloc(sizeof(void*) * array_size);
    if (!pointers) {
        printf("Failed to allocate pointers array\n");
        return;
    }
    
    printf("Testing maximum calloc allocations (128 ints each)...\n");
    
    while(1) {
        if (allocation_count >= array_size) {
            array_size *= 2;
            void **new_pointers = realloc(pointers, sizeof(void*) * array_size);
            if (!new_pointers) {
                printf("Failed to expand pointers array at count %d\n", allocation_count);
                break;
            }
            pointers = new_pointers;
        }
        
        pointers[allocation_count] = calloc(128, sizeof(int));
        if (!pointers[allocation_count]) {
            printf("calloc failed at allocation %d\n", allocation_count);
            break;
        }
        
        allocation_count++;
        
        if (allocation_count % 100 == 0) {
            printf("Allocated %d blocks so far...\n", allocation_count);
        }
    }
    
    printf("Maximum calloc allocations: %d\n", allocation_count);
    
    for (int i = 0; i < allocation_count; i++) {
        free(pointers[i]);
    }
    free(pointers);
}

void test_max_mem_palloc(void) {
    int allocation_count = 0;
    void **pointers = NULL;
    size_t array_size = 100;
    
    pointers = malloc(sizeof(void*) * array_size);
    if (!pointers) {
        printf("Failed to allocate pointers array\n");
        return;
    }
    
    printf("Testing maximum palloc allocations (1 page each)...\n");
    
    while(1) {
        if (allocation_count >= array_size) {
            array_size *= 2;
            void **new_pointers = realloc(pointers, sizeof(void*) * array_size);
            if (!new_pointers) {
                printf("Failed to expand pointers array at count %d\n", allocation_count);
                break;
            }
            pointers = new_pointers;
        }
        
        pointers[allocation_count] = palloc_get_page(PAL_ZERO);
        if (!pointers[allocation_count]) {
            printf("palloc failed at allocation %d\n", allocation_count);
            break;
        }
        
        allocation_count++;
        
        if (allocation_count % 10 == 0) {
            printf("Allocated %d pages so far...\n", allocation_count);
        }
    }
    
    printf("Maximum palloc allocations: %d\n", allocation_count);
    
    for (int i = 0; i < allocation_count; i++) {
        palloc_free_page(pointers[i]);
    }
    free(pointers);
}
