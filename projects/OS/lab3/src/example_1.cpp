#include <pthread.h>
#include <stdio.h>
#include <errno.h>

void* thread_entry(void* param)
{
	// Выполнение некоторых вычислений
	int val = 0x7fffffff, i, c = 0;
	for (i = 1; i < val; i++)
	{
		if (val % i == 0)
			c++;
	}
	return (void*)((char*)0 + c);
}

unsigned long to_ms(struct timespec* tm)
{
	return ((unsigned long)tm->tv_sec * 1000 +
			(unsigned long)tm->tv_nsec / 1000000);
}

int main()
{
	struct timespec started, finished;
	pthread_t t;
	int waits = 0;
	printf("Creating threads...\n");
	clock_gettime(CLOCK_REALTIME, &started);
	if (0 != pthread_create(&t, 0, thread_entry, 0))
	{
		printf("pthread_create failed. errno: %d\n", errno);
		return -1;
	}
	while (1)
	{
		struct timespec tm;
		int res;
		// Взять текущее время, добавить 1 сек
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += 1;
		// Ожидание завершения потока (до указанного времени)
		res = pthread_timedjoin_np(t, 0, &tm);
		waits++;

		if (res == ETIMEDOUT) // Таймаут
		{
			printf("Working: %d second(s)...\n", waits);
		}
		else if (res == 0) // Поток завершился
		{
			break;
		}
	}
	clock_gettime(CLOCK_REALTIME, &finished);

	printf("Thread finished. Execution time: %lu ms\n",
		   to_ms(&finished) - to_ms(&started));
	return 0;
}
