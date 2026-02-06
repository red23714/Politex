
/*
  File for 'smokers' task implementation.
*/

#include "devices/timer.h"
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include <stdio.h>

enum component { paper = 0, tobacco, matches };

/* --- SYNCHRONIZATION OBJECTS --- */
static struct semaphore table_empty; // semaphore = 1 when table empty
static struct semaphore sem_paper;   // smoker with paper
static struct semaphore sem_tobacco; // smoker with tobacco
static struct semaphore sem_matches; // smoker with matches

/* --- INIT --- */
static void init(void) {
  sema_init(&table_empty, 1); // table initially empty
  sema_init(&sem_paper, 0);
  sema_init(&sem_tobacco, 0);
  sema_init(&sem_matches, 0);
}

/* --- MEDIATOR THREAD --- */
static void mediator(void *arg UNUSED) {
  msg("Mediator created.");

  while (true) {
    sema_down(&table_empty); // wait until table is empty

    /* pseudo-random choice based on ticks */
    int r = timer_ticks() % 3;

    if (r == 0) {
      msg("Mediator puts TOBACCO + MATCHES.");
      sema_up(&sem_paper); // smoker with paper needs tobacco+matches
    } else if (r == 1) {
      msg("Mediator puts PAPER + MATCHES.");
      sema_up(&sem_tobacco); // smoker with tobacco needs paper+matches
    } else {
      msg("Mediator puts PAPER + TOBACCO.");
      sema_up(&sem_matches); // smoker with matches needs paper+tobacco
    }
  }
}

/* --- SMOKER THREAD --- */
static void smoker(enum component what_have) {
  char *item_names[] = {"paper", "tobacco", "matches"};
  msg("Smoker with %s created.", item_names[(int)what_have]);

  while (true) {
    /* determine which semaphore to wait on */
    if (what_have == paper)
      sema_down(&sem_paper);
    else if (what_have == tobacco)
      sema_down(&sem_tobacco);
    else
      sema_down(&sem_matches);

    msg("Smoker with %s begins smoking...", item_names[(int)what_have]);
    timer_sleep(10); // simulate smoking
    msg("Smoker with %s finished smoking.", item_names[(int)what_have]);

    sema_up(&table_empty); // notify mediator that table is free
  }
}

static void smokerw(void *arg) { smoker((enum component)arg); }

/* --- MAIN TEST ENTRY --- */
void test_smokers(void) {
  init();

  thread_create("mediator", PRI_DEFAULT, &mediator, 0);
  thread_create("smoker1", PRI_DEFAULT, &smokerw, (void *)paper);
  thread_create("smoker2", PRI_DEFAULT, &smokerw, (void *)tobacco);
  thread_create("smoker3", PRI_DEFAULT, &smokerw, (void *)matches);

  timer_msleep(5000);
  pass();
}
