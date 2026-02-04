
/* Helper functions for 'narrow_bridge' task.
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/interrupt.h"
#include "devices/timer.h"
#include <list.h>
#include "narrow-bridge.h"

// Creates threads, one thread for one car. Entry point is func
void create_vehicles(unsigned int count, thread_func* func);

// Vehicle threads entry points
void thread_normal_left(void* arg UNUSED);
void thread_normal_right(void* arg UNUSED);
void thread_emergency_left(void* arg UNUSED);
void thread_emergency_right(void* arg UNUSED);

// Calculates current number of threads
unsigned int threads_count(void);
// Helper function for threads_count
void threads_counter(struct thread *t UNUSED, void *aux);

// Wait for all vehicles threads will be terminated
void wait_threads(void);

// Vehicle entry point
void one_vehicle(enum car_priority prio, enum car_direction dir);

static unsigned int threads_count_on_start = 0;

// Test entry point
void test_narrow_bridge(unsigned int num_vehicles_left, unsigned int num_vehicles_right,
        unsigned int num_emergency_left, unsigned int num_emergency_right)
{
    threads_count_on_start = threads_count();

    narrow_bridge_init();

    create_vehicles(num_vehicles_left, thread_normal_left);
    create_vehicles(num_vehicles_right, thread_normal_right);
    create_vehicles(num_emergency_left, thread_emergency_left);
    create_vehicles(num_emergency_right, thread_emergency_right);

    wait_threads();
}

void threads_counter(struct thread *t UNUSED, void *aux)
{
   unsigned int* pcnt = (aux);
   (*pcnt)++;
}

unsigned int threads_count(void)
{
    unsigned int cnt = 0;
    enum intr_level old_level = intr_disable();
    thread_foreach(threads_counter, &cnt);
    intr_set_level (old_level);
    return cnt;
}
  
void wait_threads()
{
  while(1)
  {
    if (threads_count() > threads_count_on_start)
      thread_yield();
    else
      return;
  }
}

void create_vehicles(unsigned int count, thread_func* func)
{
     unsigned int i;
     static unsigned int car_id = 0;
     for(i = 0; i < count; i++)
     {
         char name[16];
         snprintf(name, sizeof(name), "%u", ++car_id);
         thread_create(name, PRI_DEFAULT, func, 0);
     }
}

void thread_normal_left(void* arg UNUSED)
{
    one_vehicle(car_normal, dir_left);
}

void thread_normal_right(void* arg UNUSED)
{
    one_vehicle(car_normal, dir_right);
}

void thread_emergency_left(void* arg UNUSED)
{
    one_vehicle(car_emergency, dir_left);
}

void thread_emergency_right(void* arg UNUSED)
{
    one_vehicle(car_emergency, dir_right);
}

void one_vehicle(enum car_priority prio, enum car_direction dir)
{
    arrive_bridge(prio, dir);
    cross_bridge(prio, dir);
    exit_bridge(prio, dir);
}
                                                                
void cross_bridge(enum car_priority prio, enum car_direction dir)
{
    msg("Vehicle: %4s, prio: %s, direct: %s, ticks=%4llu", 
          thread_current()->name, 
          prio == car_emergency ? "emer" : "norm",
          dir == dir_left ? "l -> r" : "l <- r",
          (unsigned long long) timer_ticks ());
    timer_sleep(10);
}
