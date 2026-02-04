/*
  File for 'smokers' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

enum component
{
   paper = 0,
   tobacco,
   matches
};

static void init(void)
{
    // Not implemented.
}

static void mediator(void* arg UNUSED)
{
    msg("Mediator created." );

    // Not implemented.
}

static void smoker(enum component what_have)
{
    char* item_names[] = { "paper", "tobacco", "matches" };
    msg("Smoker with %s created.", item_names[ (int) what_have] );

    // Not implemented    
}

static void smokerw(void* arg)
{
    smoker( (enum component) arg );
}

void test_smokers(void)
{
  init();

  thread_create("mediator", PRI_DEFAULT, &mediator, 0);
  thread_create("smoker1", PRI_DEFAULT, &smokerw, (void*) paper );
  thread_create("smoker2", PRI_DEFAULT, &smokerw, (void*) tobacco );
  thread_create("smoker3", PRI_DEFAULT, &smokerw, (void*) matches );  

  timer_msleep(5000);
  pass();
}
