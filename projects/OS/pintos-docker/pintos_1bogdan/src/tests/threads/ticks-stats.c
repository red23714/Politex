/*
  File for 'ticks-stats' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "devices/timer.h"

#define NUM_THREADS 10
#define ITERATIONS 10
#define DELAY_TICKS 100

static void infinite_loop(void *aux) {
    int thread_num = (int)(intptr_t)aux;
    
    while(1) {
        // Пустой бесконечный цикл
        asm volatile ("nop");
    }
}

void test_ticks_stats(void) {
    char name[16];
    
    printf("Starting ticks statistics test...\n");
    
    // Создаем 10 потоков
    for (int i = 0; i < NUM_THREADS; i++) {
        snprintf(name, sizeof(name), "thread_%d", i + 1);
        tid_t tid = thread_create(name, PRI_DEFAULT, infinite_loop, (void*)(intptr_t)(i + 1));
        
        if (tid == TID_ERROR) {
            printf("Failed to create thread %d\n", i + 1);
            return;
        }
    }
    
    // Выполняем 10 итераций
    for (int iter = 0; iter < ITERATIONS; iter++) {
        int64_t main_ticks_before = thread_current()->ticks_running;
        
        timer_sleep(DELAY_TICKS);
        
        printf("\n=== Iteration %d ===\n", iter + 1);
        printf("Statistics after %d ticks delay:\n", DELAY_TICKS);
        
        // Получаем список всех потоков
        struct list* all_list = thread_get_all_list();
        struct list_elem *e;
        
        // Печатаем статистику для всех потоков
        for (e = list_begin(all_list); e != list_end(all_list); e = list_next(e)) {
            struct thread *t = list_entry(e, struct thread, allelem);
            
            if (strcmp(t->name, "main") == 0) {
                int64_t ticks_delta = t->ticks_running - main_ticks_before;
                printf("Main thread: %lld ticks (gained %lld this iteration)\n", 
                       (long long)t->ticks_running, (long long)ticks_delta);
            } else if (strncmp(t->name, "thread_", 7) == 0) {
                printf("%s: %lld ticks\n", t->name, (long long)t->ticks_running);
            }
        }
    }
    
    printf("\nTest completed.\n");
}
