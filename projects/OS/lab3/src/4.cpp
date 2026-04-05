// phil.cpp
// Задание: Задача об обедающих философах
// ОС: Linux, синхронизация: Semaphore (POSIX), метод: управляющий поток
// ("слуга") Компиляция: g++ phil.cpp -D_REENTRANT -lpthread -O2 -o phil Запуск:
// ./phil TOTAL PHIL
//
// Архитектура (7 потоков: main + 5 философов + 1 слуга):
//
//   Философ i:
//     1. Думает >= PHIL мс.
//     2. Помещает сообщение REQUEST в очередь слуги, sem_post(&sem_msg).
//     3. Блокируется на sem_grant[i] — ждёт разрешения слуги.
//     4. Ест >= PHIL мс.
//     5. Помещает сообщение DONE в очередь слуги.
//     6. Повторяет, пока now_ms() < TOTAL.
//
//   Слуга:
//     - Блокируется на sem_msg (нет активного ожидания).
//     - REQUEST(i): добавляет i в FIFO-очередь ожидающих, вызывает try_grant().
//     - DONE(i): освобождает вилки i, вызывает try_grant().
//     - try_grant(): выдаёт вилки первому в FIFO у кого обе вилки свободны
//       И нет соседа раньше него в очереди (исключает голодание).
//
//   Гарантии корректности:
//     - Нет дедлока: вилки всегда берутся парой, решение принимает только
//     слуга.
//     - Нет голодания: FIFO + проверка конфликтующих соседей.
//     - Нет активного ожидания: sem_wait везде.
//     - Детерминизм: нет случайных чисел.
//
//   Семафоры:
//     sem_msg        — считающий: число сообщений в очереди слуге
//     sem_msg_mutex  — бинарный: защита буфера сообщений
//     sem_grant[5]   — бинарные: слуга -> философ[i] (разрешение)
//     sem_log        — бинарный: защита stdout

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 5

// ---------- Параметры ----------
static long long g_total_ms;
static long long g_phil_ms;
static struct timespec g_t0;

// ---------- Утилиты времени ----------

static long long now_ms()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (long long)(t.tv_sec - g_t0.tv_sec) * 1000LL +
		   (long long)(t.tv_nsec - g_t0.tv_nsec) / 1000000LL;
}

static void sleep_ms(long long ms)
{
	if (ms <= 0)
		return;
	struct timespec ts;
	ts.tv_sec = (time_t)(ms / 1000);
	ts.tv_nsec = (long)((ms % 1000) * 1000000L);
	nanosleep(&ts, NULL);
}

// ---------- Семафоры ----------
static sem_t sem_msg;		// Считающий: количество сообщений слуге
static sem_t sem_msg_mutex; // Бинарный: защита очереди сообщений
static sem_t sem_grant[N];	// Бинарные: разрешение i-му философу есть
static sem_t sem_log;		// Бинарный: защита stdout

// ---------- Очередь сообщений слуге ----------
typedef enum
{
	MSG_REQUEST = 0,
	MSG_DONE = 1,
	MSG_QUIT = 2
} MsgType;

struct Message
{
	MsgType type;
	int phil;
};

#define MSG_BUF_CAP 128
static Message g_msgbuf[MSG_BUF_CAP];
static int g_msg_head = 0;
static int g_msg_tail = 0;
static int g_msg_size = 0;

// Добавить сообщение (потокобезопасно через sem_msg_mutex)
static void msg_send(MsgType type, int phil)
{
	sem_wait(&sem_msg_mutex);
	g_msgbuf[g_msg_tail].type = type;
	g_msgbuf[g_msg_tail].phil = phil;
	g_msg_tail = (g_msg_tail + 1) % MSG_BUF_CAP;
	g_msg_size++;
	sem_post(&sem_msg_mutex);
	sem_post(&sem_msg);
}

// Извлечь сообщение (вызывается только слугой, после sem_wait(&sem_msg))
static Message msg_recv()
{
	sem_wait(&sem_msg_mutex);
	Message m = g_msgbuf[g_msg_head];
	g_msg_head = (g_msg_head + 1) % MSG_BUF_CAP;
	g_msg_size--;
	sem_post(&sem_msg_mutex);
	return m;
}

// ---------- Состояние (только слуга) ----------
static int fork_free[N]; // 1 = свободна; вилка i между философом i и (i+1)%N

// FIFO очередь ожидающих философов
static int wq[N];
static int wq_head = 0;
static int wq_tail = 0;
static int wq_size = 0;

static void wq_push(int i)
{
	wq[wq_tail] = i;
	wq_tail = (wq_tail + 1) % N;
	wq_size++;
}

// ---------- Вывод ----------
static void log_state(long long t, int i, char from, char to)
{
	sem_wait(&sem_log);
	printf("%lld:%d:%c->%c\n", t, i + 1, from, to);
	fflush(stdout);
	sem_post(&sem_log);
}

// ---------- try_grant: выдать вилки ожидающим ----------
// Философ i использует вилки: левую = i, правую = (i+1)%N
static void try_grant()
{
	// Перебираем очередь в порядке FIFO
	for (int qi = 0; qi < wq_size; qi++)
	{
		int i = wq[(wq_head + qi) % N];
		int fl = i;
		int fr = (i + 1) % N;

		// Обе вилки должны быть свободны
		if (!fork_free[fl] || !fork_free[fr])
			continue;

		// Нет ли соседа, стоящего раньше в очереди и претендующего на те же
		// вилки?
		int blocked = 0;
		for (int qj = 0; qj < qi; qj++)
		{
			int j = wq[(wq_head + qj) % N];
			int jl = j;
			int jr = (j + 1) % N;
			if (jl == fl || jl == fr || jr == fl || jr == fr)
			{
				blocked = 1;
				break;
			}
		}
		if (blocked)
			continue;

		// Выдаём вилки
		fork_free[fl] = 0;
		fork_free[fr] = 0;

		// Удаляем i из очереди: сдвигаем элементы [qi+1 .. wq_size-1] влево на
		// 1
		for (int qk = qi; qk < wq_size - 1; qk++)
			wq[(wq_head + qk) % N] = wq[(wq_head + qk + 1) % N];
		wq_tail = (wq_tail + N - 1) % N;
		wq_size--;
		qi--; // Корректируем индекс после удаления

		// Сигнализируем философу i
		sem_post(&sem_grant[i]);
	}
}

// ---------- Поток слуги ----------
void* servant_thread(void* arg)
{
	while (1)
	{
		sem_wait(&sem_msg);
		Message m = msg_recv();

		if (m.type == MSG_QUIT)
			break;

		if (m.type == MSG_REQUEST)
		{
			wq_push(m.phil);
			try_grant();
		}
		else /* MSG_DONE */
		{
			int i = m.phil;
			fork_free[i] = 1;
			fork_free[(i + 1) % N] = 1;
			try_grant();
		}
	}
	return NULL;
}

// ---------- Поток философа ----------
struct PhilArg
{
	int idx;
};

void* philosopher_thread(void* arg)
{
	int i = ((PhilArg*)arg)->idx;

	while (1)
	{
		// Думаем минимум g_phil_ms мс
		long long think_until = now_ms() + g_phil_ms;
		long long rem = think_until - now_ms();
		if (rem > 0)
			sleep_ms(rem);

		// Не начинаем есть если время вышло
		if (now_ms() >= g_total_ms)
			break;

		// Запрашиваем еду
		msg_send(MSG_REQUEST, i);

		// Ждём разрешения слуги
		sem_wait(&sem_grant[i]);

		// Переходим T -> E
		long long t_eat = now_ms();
		log_state(t_eat, i, 'T', 'E');

		// Едим минимум g_phil_ms мс
		long long eat_until = t_eat + g_phil_ms;
		rem = eat_until - now_ms();
		if (rem > 0)
			sleep_ms(rem);

		// Переходим E -> T
		long long t_think = now_ms();
		log_state(t_think, i, 'E', 'T');

		// Сообщаем слуге
		msg_send(MSG_DONE, i);
	}

	return NULL;
}

// ---------- main ----------
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s TOTAL PHIL\n", argv[0]);
		return 1;
	}
	g_total_ms = atoll(argv[1]);
	g_phil_ms = atoll(argv[2]);

	clock_gettime(CLOCK_MONOTONIC, &g_t0);

	for (int i = 0; i < N; i++)
		fork_free[i] = 1;

	sem_init(&sem_msg, 0, 0);
	sem_init(&sem_msg_mutex, 0, 1);
	sem_init(&sem_log, 0, 1);
	for (int i = 0; i < N; i++)
		sem_init(&sem_grant[i], 0, 0);

	pthread_t servant;
	pthread_create(&servant, NULL, servant_thread, NULL);

	pthread_t phils[N];
	PhilArg pargs[N];
	for (int i = 0; i < N; i++)
	{
		pargs[i].idx = i;
		pthread_create(&phils[i], NULL, philosopher_thread, &pargs[i]);
	}

	for (int i = 0; i < N; i++)
		pthread_join(phils[i], NULL);

	msg_send(MSG_QUIT, -1);
	pthread_join(servant, NULL);

	sem_destroy(&sem_msg);
	sem_destroy(&sem_msg_mutex);
	sem_destroy(&sem_log);
	for (int i = 0; i < N; i++)
		sem_destroy(&sem_grant[i]);

	return 0;
}
