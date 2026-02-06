/*
  File for 'bear' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"


static void init(unsigned int pot_size UNUSED)
{
    // Not implemented.
}

static void bear(void* arg UNUSED)
{
    msg("bear created.");

    // Not implemented.
}

static void bee(void* arg UNUSED)
{
    msg("bee %d created.", (int) arg);

    // Not implemented.
}


void test_bear(unsigned int num_bees, unsigned int pot_size)
{
  unsigned int i;
  init(pot_size);

  thread_create("bear", PRI_DEFAULT, &bear, NULL);

  for(i = 0; i < num_bees; i++)
  {
    char name[32];
    snprintf(name, sizeof(name), "bee_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &bee, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();    
}
