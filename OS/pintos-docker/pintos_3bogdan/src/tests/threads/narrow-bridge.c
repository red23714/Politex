#include "narrow-bridge.h"
#include "tests/threads/tests.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include <stdio.h>

/* --- Абстракция состояния моста --- */
struct nb_state {
  struct lock mtx;
  struct condition cv;

  int cars;          /* текущее число машин на мосту (0..2) */
  int dir;           /* текущее активное направление или NONE */
  int queued_em_l;   /* скорые в очереди слева */
  int queued_em_r;   /* скорые в очереди справа */
  int onbridge_em_l; /* скорые, едущие слева направо */
  int onbridge_em_r; /* скорые, едущие справа налево */
};

static struct nb_state S;
static const int NONE_DIR = -1;

/* --- Инициализация --- */
void narrow_bridge_init(void) {
  lock_init(&S.mtx);
  cond_init(&S.cv);

  S.cars = 0;
  S.dir = NONE_DIR;
  S.queued_em_l = S.queued_em_r = 0;
  S.onbridge_em_l = S.onbridge_em_r = 0;
}

/* --- Вспомогательные мелочи --- */
static int *queued_em(enum car_direction d) {
  return (d == dir_left) ? &S.queued_em_l : &S.queued_em_r;
}

static int *onbridge_em(enum car_direction d) {
  return (d == dir_left) ? &S.onbridge_em_l : &S.onbridge_em_r;
}

static int total_queued_em(void) { return S.queued_em_l + S.queued_em_r; }

static int total_onbridge_em(void) { return S.onbridge_em_l + S.onbridge_em_r; }

static void wait_cv(void) { cond_wait(&S.cv, &S.mtx); }

static void signal_one(void) { cond_signal(&S.cv, &S.mtx); }

static void wake_all(void) { cond_broadcast(&S.cv, &S.mtx); }

/* --- Вспомогательная логика для экстренной --- */
static void emergency_arrive(enum car_direction dir) {
  int *q = queued_em(dir);
  int *onb = onbridge_em(dir);

  (*q)++;
  while (S.cars >= 2 || (S.cars > 0 && S.dir != dir)) {
    wait_cv();
  }

  (*q)--;
  (*onb)++;

  if (S.cars == 0)
    S.dir = dir;
  S.cars++;

  /* Если в этом направлении больше не ждут скорые — можно будить всех */
  if (*q == 0)
    wake_all();
  else
    signal_one();
}

/* --- Проверка разрешения для обычной машины --- */
static bool ordinary_can_enter(enum car_direction dir) {
  bool same_dir = (S.cars > 0 && S.dir == dir);

  int wait_here = *queued_em(dir);
  int wait_other = *queued_em((dir == dir_left) ? dir_right : dir_left);
  (void)wait_other; /* не используем напрямую, но оставлено для читабельности */

  int on_here = *onbridge_em(dir);
  int total_on = total_onbridge_em();

  /* можно ехать за скорой, если:
     - в этом направлении никого не ждёт,
     - ровно одна скорая на мосту,
     - всего на мосту одна скорая,
     - направление совпадает,
     - и мост занят одной машиной (есть место для второй) */
  bool follow_em = (wait_here == 0 && on_here == 1 && total_on == 1 &&
                    same_dir && S.cars == 1);

  bool general_ok = (S.cars < 2 && (S.cars == 0 || same_dir));

  /* Разрешаем, если общие ограничения выполнены и
     либо нет скорых вообще (ни ждут, ни едут), либо выполняется follow_em */
  if (!general_ok)
    return false;
  if ((total_queued_em() == 0 && total_onbridge_em() == 0) || follow_em)
    return true;
  return false;
}

/*
  arrive_bridge:
  - максимум 2 машины на мосту;
  - одно направление;
  - скорые имеют приоритет;
  - обычные едут только если нет скорых в направлении или по правилу follow_em.
*/
void arrive_bridge(enum car_priority prio, enum car_direction dir) {
  lock_acquire(&S.mtx);

  if (prio == car_emergency) {
    emergency_arrive(dir);
    lock_release(&S.mtx);
    return;
  }

  /* Обычная машина: ждем в цикле до разрешения */
  for (;;) {
    if (ordinary_can_enter(dir))
      break;
    wait_cv();
  }

  if (S.cars == 0)
    S.dir = dir;
  S.cars++;

  lock_release(&S.mtx);
}

/*
  exit_bridge: уменьшает счётчики и пробуждает ожидателей.
*/
void exit_bridge(enum car_priority prio, enum car_direction dir) {
  lock_acquire(&S.mtx);

  int *onb = onbridge_em(dir);

  if (S.cars > 0)
    S.cars--;

  if (prio == car_emergency && *onb > 0) {
    (*onb)--;
  }

  if (S.cars == 0) {
    S.dir = NONE_DIR;
    wake_all();
  } else {
    signal_one();
  }

  lock_release(&S.mtx);
}
