/*
  File for 'molec' task implementation.
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

static void oxygen(void* arg UNUSED)
{
    msg("O created.");

    // Not implemented.
}

static void hydrogen(void* arg UNUSED)
{
    msg("H created.");

    // Not implemented.
}

void test_molec(unsigned int num_oxygen, unsigned int num_hydrogen, int interval)
{
  int last_hyd = 1;
  unsigned int oxy = 0, hyd = 0;
  init();

  while ( (oxy < num_oxygen) || (hyd < num_hydrogen) )
  {
    timer_sleep(interval);

    if (oxy < num_oxygen && (last_hyd || (num_hydrogen == hyd)) )
    {
      oxy++;
      thread_create("oxygen", PRI_DEFAULT, &oxygen, 0);
      last_hyd = 0;
    }
    else if (hyd < num_hydrogen)
    {
      hyd++;
      thread_create("hydrogen", PRI_DEFAULT, &hydrogen, 0);
      last_hyd = 1;
    }
  }

  timer_msleep(5000);
  pass();
}
