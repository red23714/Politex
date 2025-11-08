/*
  File for 'rolcoast' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

static void init(unsigned int carriage_size UNUSED)
{
    // Not implemented.
}

static void carriage(void* arg UNUSED)
{
    msg("carriage created.");

    // Not implemented.
}

static void passenger(void* arg)
{
    msg("passenger %d created.", (int) arg);

    // Not implemented.
}

void test_rolcoast(unsigned int num_clients, unsigned int carriage_size, unsigned int interval)
{
  unsigned int i;
  init(carriage_size);

  thread_create("carriage", PRI_DEFAULT, &carriage, NULL);

  for(i = 0; i < num_clients; i++)
  {
    char name[32];
    timer_sleep(interval);
    snprintf(name, sizeof(name), "passenger_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &passenger, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();    
}
