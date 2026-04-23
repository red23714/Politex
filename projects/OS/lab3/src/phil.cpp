#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define N 5 /* число философов */

/* ------------------------------------------------------------------ */
/* Глобальное время старта                                             */
/* ------------------------------------------------------------------ */
static struct timespec g_start;

static long now_ms(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	long sec = ts.tv_sec - g_start.tv_sec;
	long nsec = ts.tv_nsec - g_start.tv_nsec;
	return sec * 1000L + nsec / 1000000L;
}

static void sleep_ms(long ms)
{
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000L;
	nanosleep(&ts, NULL);
}

/* ------------------------------------------------------------------ */
/* Параметры программы                                                 */
/* ------------------------------------------------------------------ */
static long g_total_ms; /* общее время работы                       */
static long g_phil_ms;	/* минимальное время в одном состоянии      */

/* ------------------------------------------------------------------ */
/* Разделяемые данные — защищены g_mutex                              */
/* ------------------------------------------------------------------ */
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Состояние каждого философа: 0 — думает, 1 — ждёт, 2 — ест        */
static int g_state[N];

/*
 * Очередь ожидающих философов (FIFO, кольцевой буфер).
 * Каждый элемент — индекс философа (0..4).
 */
static int g_queue[N];
static int g_qhead = 0; /* индекс первого элемента                */
static int g_qtail = 0; /* индекс следующей свободной ячейки      */
static int g_qsize = 0; /* число элементов в очереди              */

/* Условные переменные: каждый философ ждёт на своей                  */
static pthread_cond_t g_cond[N];

/* Условная переменная слуги — будим его при новом запросе            */
static pthread_cond_t g_waiter_cond = PTHREAD_COND_INITIALIZER;

/* Флаг завершения: философы не берут новых вилок после его установки */
static int g_done = 0;

/* ------------------------------------------------------------------ */
/* Вспомогательные функции                                             */
/* ------------------------------------------------------------------ */

/* Левый/правый сосед (индексы 0..4)                                  */
static inline int left_of(int i) { return (i + N - 1) % N; }
static inline int right_of(int i) { return (i + 1) % N; }

/* Может ли философ i есть прямо сейчас (вилки свободны)?            */
static int can_eat(int i)
{
	return g_state[left_of(i)] != 2 && g_state[right_of(i)] != 2;
}

/* Добавить философа в конец очереди (вызывается под g_mutex)        */
static void enqueue(int i)
{
	g_queue[g_qtail] = i;
	g_qtail = (g_qtail + 1) % N;
	g_qsize++;
}

/* Удалить элемент по позиции pos из кольцевой очереди               */
static void queue_remove(int pos)
{
	/* Сдвигаем элементы от pos до хвоста на одну позицию назад      */
	int cur = pos;
	int cnt = g_qsize - 1 - (pos - g_qhead + N) % N;
	/* Проще: переписываем кольцо, пропуская pos                     */
	int idx = g_qhead;
	int new_head = g_qhead;
	int new_tail = g_qhead;
	int tmp[N];
	int sz = 0;
	for (int k = 0; k < g_qsize; k++)
	{
		int slot = (g_qhead + k) % N;
		if (slot != pos)
		{
			tmp[sz++] = g_queue[slot];
		}
	}
	for (int k = 0; k < sz; k++)
	{
		g_queue[(g_qhead + k) % N] = tmp[k];
	}
	g_qtail = (g_qhead + sz) % N;
	g_qsize = sz;
	(void)cur;
	(void)cnt;
	(void)idx;
	(void)new_head;
	(void)new_tail;
}

/* Вывод смены состояния                                              */
static void print_change(int num1based, char old_state, char new_state)
{
	printf("%ld:%d:%c->%c\n", now_ms(), num1based, old_state, new_state);
	fflush(stdout);
}

/* ------------------------------------------------------------------ */
/* Поток слуги                                                         */
/* ------------------------------------------------------------------ */
static void* waiter_thread(void* arg)
{
	(void)arg;
	pthread_mutex_lock(&g_mutex);
	while (1)
	{
		/* Ждём, пока в очереди есть ждущие                          */
		while (g_qsize == 0 && !g_done)
		{
			pthread_cond_wait(&g_waiter_cond, &g_mutex);
		}

		if (g_qsize == 0 && g_done)
		{
			break;
		}

		/*
		 * Проходим очередь в порядке FIFO и выдаём разрешение первому
		 * философу, у которого оба соседа не едят.
		 */
		int granted = 0;
		for (int k = 0; k < g_qsize; k++)
		{
			int slot = (g_qhead + k) % N;
			int i = g_queue[slot];
			if (can_eat(i))
			{
				/* Выдаём разрешение                                  */
				g_state[i] = 2; /* eating                   */
				queue_remove(slot);
				pthread_cond_signal(&g_cond[i]);
				granted = 1;
				break;
			}
		}

		if (!granted)
		{
			/*
			 * Никому из ждущих нельзя есть прямо сейчас.
			 * Ждём сигнала от философа, завершившего еду.
			 */
			pthread_cond_wait(&g_waiter_cond, &g_mutex);
		}
	}
	pthread_mutex_unlock(&g_mutex);
	return NULL;
}

/* ------------------------------------------------------------------ */
/* Поток философа                                                       */
/* ------------------------------------------------------------------ */
typedef struct
{
	int idx; /* 0..4 */
} PhilArg;

static void* phil_thread(void* arg)
{
	int i = ((PhilArg*)arg)->idx;
	int num = i + 1; /* номер для вывода (1..5) */

	while (1)
	{
		/* ---- Thinking ---- */
		sleep_ms(g_phil_ms);

		/* Проверяем время — если уже можно завершать, выходим       */
		pthread_mutex_lock(&g_mutex);
		if (g_done)
		{
			pthread_mutex_unlock(&g_mutex);
			break;
		}

		/* Переходим в состояние "хочет есть", встаём в очередь      */
		g_state[i] = 1;
		enqueue(i);
		pthread_cond_signal(&g_waiter_cond);

		/* Ждём разрешения слуги                                      */
		while (g_state[i] != 2 && !g_done)
		{
			pthread_cond_wait(&g_cond[i], &g_mutex);
		}

		/* Если проснулись из-за g_done, а не разрешения есть       */
		if (g_state[i] != 2)
		{
			pthread_mutex_unlock(&g_mutex);
			break;
		}

		/* Слуга дал разрешение — g_state[i] == 2                   */
		pthread_mutex_unlock(&g_mutex);

		/* Сообщаем о смене состояния T->E */
		print_change(num, 'T', 'E');

		/* ---- Eating ---- */
		sleep_ms(g_phil_ms);

		/* ---- Заканчиваем есть ---- */
		pthread_mutex_lock(&g_mutex);
		g_state[i] = 0; /* thinking */
		/* Будим слугу — возможно, кто-то из ждущих теперь может есть */
		pthread_cond_signal(&g_waiter_cond);
		pthread_mutex_unlock(&g_mutex);

		/* Сообщаем о смене состояния E->T */
		print_change(num, 'E', 'T');
	}

	return NULL;
}

/* ------------------------------------------------------------------ */
/* Поток таймера завершения                                            */
/* ------------------------------------------------------------------ */
static void* timer_thread(void* arg)
{
	(void)arg;
	sleep_ms(g_total_ms);

	pthread_mutex_lock(&g_mutex);
	g_done = 1;
	/* Будим слугу, чтобы он вышел из ожидания                       */
	pthread_cond_signal(&g_waiter_cond);
	pthread_mutex_unlock(&g_mutex);

	return NULL;
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s TOTAL PHIL\n", argv[0]);
		return 1;
	}

	g_total_ms = atol(argv[1]);
	g_phil_ms = atol(argv[2]);

	if (g_total_ms <= 0 || g_phil_ms <= 0)
	{
		fprintf(stderr, "Arguments must be positive integers\n");
		return 1;
	}

	/* Инициализация условных переменных                              */
	for (int i = 0; i < N; i++)
	{
		pthread_cond_init(&g_cond[i], NULL);
		g_state[i] = 0;
	}

	clock_gettime(CLOCK_MONOTONIC, &g_start);

	/* Порождаем потоки                                               */
	pthread_t phil_tid[N];
	PhilArg phil_arg[N];
	pthread_t waiter_tid;
	pthread_t timer_tid;

	pthread_create(&waiter_tid, NULL, waiter_thread, NULL);

	for (int i = 0; i < N; i++)
	{
		phil_arg[i].idx = i;
		pthread_create(&phil_tid[i], NULL, phil_thread, &phil_arg[i]);
	}

	pthread_create(&timer_tid, NULL, timer_thread, NULL);

	/* Ждём завершения таймера                                         */
	pthread_join(timer_tid, NULL);

	/*
	 * После установки g_done слуга выходит из своего цикла.
	 * Философы, ждущие разрешения (g_state==1), висят на g_cond[i].
	 * Переводим их обратно в thinking (0) и будим — они увидят
	 * g_done и завершатся без перехода в E.
	 */
	pthread_mutex_lock(&g_mutex);
	for (int i = 0; i < N; i++)
	{
		if (g_state[i] == 1)
		{
			g_state[i] = 0;
			pthread_cond_signal(&g_cond[i]);
		}
	}
	pthread_mutex_unlock(&g_mutex);

	/* Ждём всех философов                                            */
	for (int i = 0; i < N; i++)
	{
		pthread_join(phil_tid[i], NULL);
	}

	/* Ждём слугу                                                     */
	pthread_join(waiter_tid, NULL);

	/* Финальная проверка: все должны быть в T                        */
	for (int i = 0; i < N; i++)
	{
		pthread_cond_destroy(&g_cond[i]);
	}

	return 0;
}
