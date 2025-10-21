#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "threads/thread.h"
#include "threads/vaddr.h"

#define MAX_THREADS 1000  // Максимальное количество для безопасности

static void
thread_function (void *aux UNUSED) 
{
  while (1) 
  {
    thread_yield();
  }
}

void
test_max_threads (void) 
{
  tid_t threads[MAX_THREADS];
  int count = 0;
  
  printf("Testing maximum number of threads...\n");
  
  for (count = 0; count < MAX_THREADS; count++) 
  {
    char name[16];
    snprintf(name, sizeof(name), "thread_%d", count);
    
    threads[count] = thread_create(name, PRI_DEFAULT, thread_function, NULL);
    
    if (threads[count] == TID_ERROR) 
    {
      printf("Failed to create thread %d\n", count);
      break;
    }
  }
  
  printf("Maximum number of threads created: %d\n", count);
  
  // Бесконечное ожидание (тест должен быть остановлен вручную)
  while (1) 
  {
    thread_yield();
  }
}
