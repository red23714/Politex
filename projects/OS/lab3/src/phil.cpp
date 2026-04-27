#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 5

static struct timespec g_start;

static long now_ms(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec - g_start.tv_sec) * 1000L +
		   (ts.tv_nsec - g_start.tv_nsec) / 1000000L;
}

static void sleep_ms(long ms)
{
	struct timespec ts = {ms / 1000, (ms % 1000) * 1000000L};
	nanosleep(&ts, NULL);
}

static long g_total_ms;
static long g_phil_ms;

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Состояние: 0=думает, 1=ждёт, 2=ест */
static int g_state[N];

/* Простая FIFO-очередь */
static int g_queue[N];
static int g_qsize = 0;

static pthread_cond_t g_cond[N];
static pthread_cond_t g_waiter_cond = PTHREAD_COND_INITIALIZER;
static int g_done = 0;

static inline int left_of(int i) { return (i + N - 1) % N; }
static inline int right_of(int i) { return (i + 1) % N; }

static int can_eat(int i)
{
	return g_state[left_of(i)] != 2 && g_state[right_of(i)] != 2;
}

static void enqueue(int i) { g_queue[g_qsize++] = i; }

static void queue_remove(int pos)
{
	for (int k = pos; k < g_qsize - 1; k++)
		g_queue[k] = g_queue[k + 1];
	g_qsize--;
}

/* ------------------------------------------------------------------ */
/* Слуга                                                               */
/* ------------------------------------------------------------------ */
static void* waiter_thread(void* arg)
{
	(void)arg;
	pthread_mutex_lock(&g_mutex);

	while (1)
	{
		/*
		 * Спим только если очередь пуста И программа не завершается.
		 * Условие в while: просыпаемся как только g_qsize>0 или g_done.
		 */
		while (g_qsize == 0 && !g_done)
			pthread_cond_wait(&g_waiter_cond, &g_mutex);

		/* Очередь пуста и пора заканчивать */
		if (g_qsize == 0)
			break;

		/* Пробуем выдать вилки первому подходящему в очереди */
		int granted = 0;
		for (int k = 0; k < g_qsize; k++)
		{
			int i = g_queue[k];
			if (can_eat(i))
			{
				g_state[i] = 2;
				queue_remove(k);
				printf("%ld:%d:T->E\n", now_ms(), i + 1);
				fflush(stdout);
				pthread_cond_signal(&g_cond[i]);
				granted = 1;
				break;
			}
		}

		if (!granted)
		{
			/*
			 * Никто не может есть — ждём сигнала.
			 * Сигнал придёт когда:
			 *  - философ закончит есть (освободит вилки)
			 *  - философ выйдет из очереди при g_done
			 *  - таймер выставит g_done
			 * Во всех трёх случаях вызывается
			 * pthread_cond_signal(&g_waiter_cond).
			 */
			pthread_cond_wait(&g_waiter_cond, &g_mutex);
			/* После пробуждения снова идём на начало цикла —
			   там проверим g_done и g_qsize */
		}
	}

	pthread_mutex_unlock(&g_mutex);
	return NULL;
}

/* ------------------------------------------------------------------ */
/* Философ                                                             */
/* ------------------------------------------------------------------ */
typedef struct
{
	int idx;
} PhilArg;

static void* phil_thread(void* arg)
{
	int i = ((PhilArg*)arg)->idx;
	int num = i + 1;

	while (1)
	{
		sleep_ms(g_phil_ms);

		pthread_mutex_lock(&g_mutex);
		if (g_done)
		{
			pthread_mutex_unlock(&g_mutex);
			break;
		}

		g_state[i] = 1;
		enqueue(i);
		pthread_cond_signal(&g_waiter_cond);

		while (g_state[i] != 2 && !g_done)
			pthread_cond_wait(&g_cond[i], &g_mutex);

		if (g_state[i] != 2)
		{
			/* Завершение: убираем себя из очереди и будим слугу */
			for (int k = 0; k < g_qsize; k++)
				if (g_queue[k] == i)
				{
					queue_remove(k);
					break;
				}
			g_state[i] = 0;
			/* ВАЖНО: будим слугу, чтобы он пересчитал g_qsize */
			pthread_cond_signal(&g_waiter_cond);
			pthread_mutex_unlock(&g_mutex);
			break;
		}

		/* Получили разрешение есть */
		pthread_mutex_unlock(&g_mutex);

		sleep_ms(g_phil_ms);

		pthread_mutex_lock(&g_mutex);
		g_state[i] = 0;
		printf("%ld:%d:E->T\n", now_ms(), num);
		fflush(stdout);
		/* Будим слугу: сосед мог ждать вилок */
		pthread_cond_signal(&g_waiter_cond);
		pthread_mutex_unlock(&g_mutex);
	}

	return NULL;
}

/* ------------------------------------------------------------------ */
/* Таймер                                                              */
/* ------------------------------------------------------------------ */
static void* timer_thread(void* arg)
{
	(void)arg;
	sleep_ms(g_total_ms);

	pthread_mutex_lock(&g_mutex);
	g_done = 1;
	/* Будим слугу и всех ждущих философов */
	pthread_cond_broadcast(&g_waiter_cond);
	for (int i = 0; i < N; i++)
		pthread_cond_signal(&g_cond[i]);
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

	for (int i = 0; i < N; i++)
	{
		pthread_cond_init(&g_cond[i], NULL);
		g_state[i] = 0;
	}

	clock_gettime(CLOCK_MONOTONIC, &g_start);

	pthread_t waiter_tid, phil_tid[N], timer_tid;
	PhilArg phil_arg[N];

	pthread_create(&waiter_tid, NULL, waiter_thread, NULL);
	for (int i = 0; i < N; i++)
	{
		phil_arg[i].idx = i;
		pthread_create(&phil_tid[i], NULL, phil_thread, &phil_arg[i]);
	}
	pthread_create(&timer_tid, NULL, timer_thread, NULL);

	pthread_join(timer_tid, NULL);
	for (int i = 0; i < N; i++)
		pthread_join(phil_tid[i], NULL);
	pthread_join(waiter_tid, NULL);

	for (int i = 0; i < N; i++)
		pthread_cond_destroy(&g_cond[i]);

	return 0;
}
