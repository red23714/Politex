/*
  File for 'hair' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

static void init(void)
{
    // Not implemented.
}

static void hairdresser(void* arg UNUSED)
{
    msg("hairdresser created.");

    // Not implemented.
}

static void client(void* arg UNUSED)
{
    msg("client %d created.", (int) arg);

    // Not implemented.
}

void test_hair(unsigned int num_clients, unsigned int interval)
{
  unsigned int i;
  init();

  thread_create("hairdresser", PRI_DEFAULT, &hairdresser, NULL);

  for(i = 0; i < num_clients; i++)
  {
    char name[32];
    timer_sleep(interval);
    snprintf(name, sizeof(name), "client_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &client, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();
}
