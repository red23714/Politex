/*
  File for 'max-rec-calls' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"

static int call_count = 0;

static void recursive_function(int a, int b) {
    call_count++;
    
    // Выводим каждые 100 вызовов для отслеживания прогресса
    if (call_count % 25 == 0) {
        printf("Recursive call count: %d\n", call_count);
    }
    
    // Рекурсивный вызов
    recursive_function(a + 1, b - 1);
}

void test_max_rec_calls(void) {
    call_count = 0;
    printf("Testing maximum recursive calls...\n");
    
    // Начинаем рекурсию
    recursive_function(0, 0);
    
    // Этот код никогда не выполнится из-за переполнения стека
    printf("Final count: %d\n", call_count);
}
