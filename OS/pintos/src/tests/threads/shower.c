/*
  File for 'shower' task implementation.
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

static void man(void* arg UNUSED)
{
    msg("man %d created.", (int) arg);

    // Not implemented.
}

static void woman(void* arg UNUSED)
{
    msg("woman %d created.", (int) arg);

    // Not implemented.
}

void test_shower(unsigned int num_men, unsigned int num_women, unsigned int interval)
{
  int last_women = 1;
  unsigned int men = 0, women = 0;
  init();

  while ( (men < num_men) || (women < num_women) )
  {
    char name[32];
    timer_sleep(interval);

    if (men < num_men && (last_women || (num_women == women)) )
    {
      men++;
      snprintf(name, sizeof(name), "man_%d", men);      
      thread_create(name, PRI_DEFAULT, &man, (void*) men);
      last_women = 0;
    }
    else if (women < num_women)
    {
      women++;
      snprintf(name, sizeof(name), "woman_%d", women);      
      thread_create(name, PRI_DEFAULT, &woman, (void*) women);
      last_women = 1;
    }
  }

  timer_msleep(5000);
  pass();
}
