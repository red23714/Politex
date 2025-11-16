#include "narrow-bridge.h"
#include "tests/threads/tests.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include <stdio.h>

/* Синхронизация */
static struct lock bridge_lock;
static struct condition can_enter;

/* Состояние моста */
static int cars_on_bridge = 0;
static enum car_direction current_dir = -1;

/* Счётчики скорых по направлениям */
static int waiting_emergency_left = 0;
static int waiting_emergency_right = 0;
static int emergencies_on_bridge_left = 0;
static int emergencies_on_bridge_right = 0;

void narrow_bridge_init(void) {
  lock_init(&bridge_lock);
  cond_init(&can_enter);

  cars_on_bridge = 0;
  current_dir = -1;
  waiting_emergency_left = waiting_emergency_right = 0;
  emergencies_on_bridge_left = emergencies_on_bridge_right = 0;
}

/* Вспомогательные функции */
static int *waiting_emergency_dir(enum car_direction dir) {
  return dir == dir_left ? &waiting_emergency_left : &waiting_emergency_right;
}

static int *emergencies_on_bridge_dir(enum car_direction dir) {
  return dir == dir_left ? &emergencies_on_bridge_left
                         : &emergencies_on_bridge_right;
}

static int total_waiting_emergency() {
  return waiting_emergency_left + waiting_emergency_right;
}

static int total_emergencies_on_bridge() {
  return emergencies_on_bridge_left + emergencies_on_bridge_right;
}

/*
    arrive_bridge():
    - максимум 2 машины на мосту;
    - движение только в одном направлении;
    - скорые приоритетны;
    - если в направлении больше нет скорых, можно ехать с обычной.
*/
void arrive_bridge(enum car_priority prio, enum car_direction dir) {
  lock_acquire(&bridge_lock);

  int *waiting_same = waiting_emergency_dir(dir);
  int *on_bridge_same = emergencies_on_bridge_dir(dir);

  if (prio == car_emergency) {
    (*waiting_same)++;

    while (cars_on_bridge >= 2 || (cars_on_bridge > 0 && current_dir != dir)) {
      cond_wait(&can_enter, &bridge_lock);
    }

    (*waiting_same)--;
    (*on_bridge_same)++;

    if (cars_on_bridge == 0)
      current_dir = dir;

    cars_on_bridge++;

    /* Разрешаем обычным ехать, если больше нет скорых в этом направлении */
    if (*waiting_same == 0)
      cond_broadcast(&can_enter, &bridge_lock);
    else
      cond_signal(&can_enter, &bridge_lock);

    lock_release(&bridge_lock);
    return;
  }

  /* Обычные машины */
  while (true) {
    bool same_dir = (cars_on_bridge > 0 && current_dir == dir);

    int waiting_same_dir = *waiting_emergency_dir(dir);
    int waiting_opposite =
        *waiting_emergency_dir(dir == dir_left ? dir_right : dir_left);
    int on_bridge_same_dir = *emergencies_on_bridge_dir(dir);
    int on_bridge_total = total_emergencies_on_bridge();

    /* можно ехать со скорой, если:
       - нет скорых в том же направлении (ждущих или едущих больше 1),
       - есть ровно одна скорая на мосту,
       - направление совпадает,
       - есть место. */
    bool can_go_with_emergency =
        (waiting_same_dir == 0 && on_bridge_same_dir == 1 &&
         on_bridge_total == 1 && same_dir && cars_on_bridge == 1);

    if (cars_on_bridge < 2 && (cars_on_bridge == 0 || same_dir) &&
        ((total_waiting_emergency() == 0 &&
          total_emergencies_on_bridge() == 0) ||
         can_go_with_emergency)) {
      break;
    }

    cond_wait(&can_enter, &bridge_lock);
  }

  if (cars_on_bridge == 0)
    current_dir = dir;

  cars_on_bridge++;
  lock_release(&bridge_lock);
}

/*
    exit_bridge():
*/
void exit_bridge(enum car_priority prio, enum car_direction dir) {
  lock_acquire(&bridge_lock);

  int *on_bridge_same = emergencies_on_bridge_dir(dir);

  if (cars_on_bridge > 0)
    cars_on_bridge--;

  if (prio == car_emergency && *on_bridge_same > 0)
    (*on_bridge_same)--;

  if (cars_on_bridge == 0) {
    current_dir = -1;
    cond_broadcast(&can_enter, &bridge_lock);
  } else {
    cond_signal(&can_enter, &bridge_lock);
  }

  lock_release(&bridge_lock);
}
