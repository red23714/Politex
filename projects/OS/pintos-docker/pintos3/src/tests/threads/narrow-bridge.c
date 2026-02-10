
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

// Called before test. Can initialize some synchronization objects.
void narrow_bridge_init(void)
{
  lock_init(&bridge_lock);
    list_init(&emer_order);
    list_init(&norm_order);
    cars_on_bridge = 0;
    current_dir = -1;
    emer_in_left_order = 0;
    emer_in_right_order = 0;
    goida = 0;
}

void arrive_bridge(enum car_priority prio, enum car_direction dir)
{
    bool will_block = false;
    if(prio == car_emergency) {
      if(!list_empty(&emer_order)){
        will_block = true;
      }
    }
    else {
      if(!list_empty(&emer_order) || !list_empty(&norm_order)){
        will_block = true;
      }
    }

    lock_acquire(&bridge_lock);
    if (will_block == false && cars_on_bridge < MAX_ON_BRIDGE &&
        (cars_on_bridge == 0 || current_dir == dir)) {
        cars_on_bridge++;
        current_dir = dir;
    } else {
        will_block = true;
    }
    lock_release(&bridge_lock);
    if (will_block){
        struct waiter_of_bridge *wbr = malloc(sizeof *wbr);
        wbr->dir = dir;
        wbr->prio = prio;
        sema_init(&wbr->sem, 0);
        if (prio == car_emergency){
            list_push_back(&emer_order, &wbr->elem);
            if(dir == dir_left)emer_in_left_order++;
            else emer_in_right_order++;
        }
        else
            list_push_back(&norm_order, &wbr->elem);
        sema_down(&wbr->sem);
        free(wbr);
    }
    else {
      goida++;
    }
}

static int wake_k_from_order(struct list *order, enum car_direction dir, int k) {
    int w = 0;
    struct list_elem *e = list_begin(order);
    while (e != list_end(order) && w < k) {
        struct list_elem *next = list_next(e);
        struct waiter_of_bridge *wbr = list_entry(e, struct waiter_of_bridge, elem);
        if (wbr->dir == dir) {
            list_remove(e);
            cars_on_bridge++;
            current_dir = dir;
            sema_up(&wbr->sem);
            w++;
            if(wbr->prio == car_emergency){
              if(dir == dir_left)emer_in_left_order--;
              else emer_in_right_order--;
            }
        }
        e = next;
    }
    return w;
}

void exit_bridge(enum car_priority prio, enum car_direction dir)
{
  lock_acquire(&bridge_lock);
        cars_on_bridge--;
    if (cars_on_bridge == 0){
        current_dir = -1;
        goida = 0;
    }
    int free_slots = MAX_ON_BRIDGE - cars_on_bridge;
    int awakened = 0;
    if (!list_empty(&emer_order)) {
        struct waiter_of_bridge *first =
            list_entry(list_front(&emer_order), struct waiter_of_bridge, elem);
        enum car_direction want_dir = first->dir;
        if (cars_on_bridge == 0 || (cars_on_bridge == 1 && current_dir == want_dir))
            awakened += wake_k_from_order(&emer_order, want_dir, free_slots);
        goida += awakened;
        free_slots -= awakened;
        awakened = 0;
        //printf("awakened:%d, goida:%d, emer_in_left:%d, emer_in_right:%d\n", awakened, goida, emer_in_left_order, emer_in_right_order);
        if(free_slots != 0 && goida % 2 == 1){
          if(current_dir == dir_left && emer_in_left_order == 0){
            emer_in_left_order--;
            wake_k_from_order(&norm_order, current_dir, free_slots);
            }
          if(current_dir == dir_right && emer_in_right_order == 0){
            emer_in_right_order--;
            wake_k_from_order(&norm_order, current_dir, free_slots);
          }
        }
        goida += awakened;
    }
    else if(!list_empty(&norm_order))
    {
        //printf("goida:%d, emer_in_left:%d, emer_in_right:%d\n",goida, emer_in_left_order, emer_in_right_order);
        if(goida % 2 == 1){
          if(current_dir == dir_left && emer_in_left_order == 0){
            emer_in_left_order--;
            wake_k_from_order(&norm_order, current_dir, free_slots);
            goida = 0;
            }
          if(current_dir == dir_right && emer_in_right_order == 0){
            emer_in_right_order--;
            wake_k_from_order(&norm_order, current_dir, free_slots);
            goida = 0;
          }
        }
        struct waiter_of_bridge *first =
            list_entry(list_front(&norm_order), struct waiter_of_bridge, elem);
        enum car_direction want_dir = first->dir;
        if (cars_on_bridge == 0 || (cars_on_bridge == 1 && current_dir == want_dir))
            wake_k_from_order(&norm_order, want_dir, free_slots - awakened);
    }
    lock_release(&bridge_lock);
}
