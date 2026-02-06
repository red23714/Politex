/*
  File for 'birds' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

static void init(unsigned int dish_size UNUSED)
{
    // Not implemented.
}

static void bird(void* arg UNUSED)
{
    msg("bird created.");

    // Not implemented.
}

static void chick(void* arg)
{
    msg("chick %d created.", (int) arg);

    // Not implemented.
}


void test_birds(unsigned int num_chicks, unsigned int dish_size)
{
  unsigned int i;
  init(dish_size);

  thread_create("bird", PRI_DEFAULT, &bird, NULL);

  for(i = 0; i < num_chicks; i++)
  {
    char name[32];
    snprintf(name, sizeof(name), "chick_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &chick, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();    
}
