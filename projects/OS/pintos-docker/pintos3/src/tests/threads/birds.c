/*
  File for 'birds' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"


static struct semaphore food_mutex;
static struct semaphore mother_sleep;
static struct semaphore food_available;

static int food_portions;
static int max_food_portions;

static void init(unsigned int dish_size)
{
    sema_init(&food_mutex, 1);
    sema_init(&mother_sleep, 0);
    sema_init(&food_available, dish_size);
    max_food_portions = dish_size;
    food_portions = dish_size;
}

static void bird(void* arg)
{
    msg("bird created.");
    while (true) {
        sema_down(&mother_sleep);
        sema_down(&food_mutex);
        food_portions = max_food_portions;
        msg("The mother filled the bowls. Quantity of dishes: %d\n", food_portions);
        sema_up(&food_mutex);

        for (int i = 0; i < max_food_portions; i++) {
            sema_up(&food_available);
        }
    }
}

static void chick(void* arg)
{
    msg("chick %d created.", (int) arg);
    while (true) {
        sema_down(&food_available);
        sema_down(&food_mutex);

        food_portions--;
        msg("%s ate dish, left %d\n", thread_current()->name, food_portions);

        if (food_portions == 0) {
            sema_up(&mother_sleep);
        }

        sema_up(&food_mutex);
        timer_sleep(100);
    }

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
