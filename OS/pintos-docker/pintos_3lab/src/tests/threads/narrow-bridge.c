#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

/* Синхронизация */
static struct lock bridge_lock;
static struct condition can_enter;

/* Состояние моста */
static int cars_on_bridge = 0;
static enum car_direction current_dir = -1;
static int waiting_emergency = 0;     // сколько скорых ждут
static int emergencies_on_bridge = 0; // сколько скорых сейчас на мосту

void narrow_bridge_init(void) {
    lock_init(&bridge_lock);
    cond_init(&can_enter);
    cars_on_bridge = 0;
    current_dir = -1;
    waiting_emergency = 0;
    emergencies_on_bridge = 0;
}

/*
    arrive_bridge():
    - не более двух машин одновременно;
    - движение только в одном направлении;
    - скорые имеют приоритет;
    - обычные едут только если нет скорых в очереди или на мосту.
*/
void arrive_bridge(enum car_priority prio, enum car_direction dir) {
    lock_acquire(&bridge_lock);

    if (prio == car_emergency) {
        /* Скорая входит вне очереди */
        waiting_emergency++;

        /* Ждёт, пока не освободится место и направление совпадает или мост пуст */
        while (cars_on_bridge >= 2 ||
               (cars_on_bridge > 0 && current_dir != dir)) {
            cond_wait(&can_enter, &bridge_lock);
        }

        waiting_emergency--;
        emergencies_on_bridge++;

        if (cars_on_bridge == 0)
            current_dir = dir;

        cars_on_bridge++;

        /* Разрешаем другим скорым того же направления ехать вместе */
        cond_broadcast(&can_enter, &bridge_lock);

        lock_release(&bridge_lock);
        return;
    }

    /* Обычные машины */
    while (
        /* скорые ждут или едут — обычным нельзя */
        waiting_emergency > 0 ||
        emergencies_on_bridge > 0 ||
        /* мост заполнен */
        cars_on_bridge >= 2 ||
        /* направление другое */
        (cars_on_bridge > 0 && current_dir != dir)
    ) {
        cond_wait(&can_enter, &bridge_lock);
    }

    if (cars_on_bridge == 0)
        current_dir = dir;

    cars_on_bridge++;
    lock_release(&bridge_lock);
}

/*
    exit_bridge():
    - освобождает место на мосту;
    - если мост стал пустым — разрешает смену направления.
*/
void exit_bridge(enum car_priority prio, enum car_direction dir) {
    lock_acquire(&bridge_lock);

    if (cars_on_bridge > 0)
        cars_on_bridge--;

    if (prio == car_emergency && emergencies_on_bridge > 0)
        emergencies_on_bridge--;

    /* Если мост пуст — меняем направление и будим всех */
    if (cars_on_bridge == 0) {
        current_dir = -1;
        cond_broadcast(&can_enter, &bridge_lock);
    } else {
        cond_signal(&can_enter, &bridge_lock);
    }

    lock_release(&bridge_lock);
}
