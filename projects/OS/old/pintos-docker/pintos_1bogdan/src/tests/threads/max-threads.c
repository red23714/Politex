/*
  File for 'max-threads' task implementation.
*/

#include <stdio.h>
#include <stdlib.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"

#define STACK_SIZE 1024

static void dummy_function(void *aux UNUSED) {
    while(1) {
        thread_yield();
    }
}

void test_max_threads(void) {
    int thread_count = 0;
    
    printf("Testing maximum number of threads...\n");
    
    while(1) {
        char name[16];
        snprintf(name, sizeof(name), "thread_%d", thread_count);
        
        tid_t tid = thread_create(name, PRI_DEFAULT, dummy_function, NULL);
        
        if (tid == TID_ERROR) {
            printf("Failed to create thread %d\n", thread_count);
            break;
        }
        
        thread_count++;
        
        // Периодически выводим прогресс
        if (thread_count % 10 == 0) {
            printf("Created %d threads so far...\n", thread_count);
        }
    }
    
    printf("Maximum number of threads: %d\n", thread_count);
    printf("This is limited by available kernel heap memory for thread structures\n");
}
