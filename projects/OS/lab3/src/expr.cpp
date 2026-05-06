#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

typedef void (*task_func_t)(void* arg);

typedef struct
{
	task_func_t func;
	void* arg;
} task_t;

typedef struct queue_elem
{
	struct queue_elem* next;
	task_t task;
} queue_elem;

typedef struct queue_t
{
	queue_elem* head;
	queue_elem* tail;
	int length;
	bool is_running;
	int active_threads;

#ifdef _WIN32
	HANDLE mutex;
	HANDLE done_event;
	HANDLE semaphore;
#else
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	sem_t semaphore;
#endif

} queue_t;

//////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////

int queue_init(queue_t* q)
{
	if (!q)
		return -1;

	q->head = NULL;
	q->tail = NULL;
	q->length = 0;
	q->is_running = true;
	q->active_threads = 0;

#ifdef _WIN32
	q->mutex = CreateMutex(NULL, FALSE, NULL);
	q->done_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	q->semaphore = CreateSemaphore(NULL, 0, LONG_MAX, NULL);

	if (!q->mutex || !q->done_event || !q->semaphore)
		return -1;
#else
	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->cond, NULL);
	sem_init(&q->semaphore, 0, 0);
#endif

	return 0;
}

void queue_destroy(queue_t* q)
{
#ifdef _WIN32
	WaitForSingleObject(q->mutex, INFINITE);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	while (q->head)
	{
		queue_elem* tmp = q->head;
		q->head = q->head->next;
		free(tmp);
	}

#ifdef _WIN32
	ReleaseMutex(q->mutex);
	CloseHandle(q->mutex);
	CloseHandle(q->done_event);
	CloseHandle(q->semaphore);
#else
	pthread_mutex_unlock(&q->mutex);
	pthread_mutex_destroy(&q->mutex);
	pthread_cond_destroy(&q->cond);
	sem_destroy(&q->semaphore);
#endif
}

task_t queue_pop(queue_t* q)
{
#ifdef _WIN32
	WaitForSingleObject(q->semaphore, INFINITE);
	WaitForSingleObject(q->mutex, INFINITE);
#else
	sem_wait(&q->semaphore);
	pthread_mutex_lock(&q->mutex);
#endif

	if (!q->head)
	{
#ifdef _WIN32
		ReleaseMutex(q->mutex);
#else
		pthread_mutex_unlock(&q->mutex);
#endif
		task_t empty = {NULL, NULL};
		return empty;
	}

	task_t t = q->head->task;
	queue_elem* tmp = q->head;
	q->head = q->head->next;

	if (!q->head)
		q->tail = NULL;

	q->length--;
	free(tmp);

#ifdef _WIN32
	ReleaseMutex(q->mutex);
#else
	pthread_mutex_unlock(&q->mutex);
#endif

	return t;
}

void queue_push(queue_t* q, task_func_t func, void* arg)
{
	queue_elem* tmp = (queue_elem*)malloc(sizeof(queue_elem));
	if (!tmp)
		return;

	tmp->task.func = func;
	tmp->task.arg = arg;
	tmp->next = NULL;

#ifdef _WIN32
	WaitForSingleObject(q->mutex, INFINITE);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	if (!q->is_running)
	{
		free(tmp);
#ifdef _WIN32
		ReleaseMutex(q->mutex);
#else
		pthread_mutex_unlock(&q->mutex);
#endif
		return;
	}

	if (!q->tail)
		q->head = q->tail = tmp;
	else
	{
		q->tail->next = tmp;
		q->tail = tmp;
	}

	q->length++;

#ifdef _WIN32
	ReleaseMutex(q->mutex);
	ReleaseSemaphore(q->semaphore, 1, NULL);
#else
	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->semaphore);
#endif
}

#ifdef _WIN32
DWORD WINAPI worker(LPVOID arg)
#else
void* worker(void* arg)
#endif
{
	queue_t* q = (queue_t*)arg;

#ifdef _WIN32
	WaitForSingleObject(q->mutex, INFINITE);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	q->active_threads++;

#ifdef _WIN32
	ReleaseMutex(q->mutex);
#else
	pthread_mutex_unlock(&q->mutex);
#endif

	while (1)
	{
#ifdef _WIN32
		WaitForSingleObject(q->mutex, INFINITE);
#else
		pthread_mutex_lock(&q->mutex);
#endif

		bool should_stop = !q->is_running && q->length == 0;

#ifdef _WIN32
		ReleaseMutex(q->mutex);
#else
		pthread_mutex_unlock(&q->mutex);
#endif

		if (should_stop)
			break;

		task_t t = queue_pop(q);
		if (t.func)
			t.func(t.arg);
	}

#ifdef _WIN32
	WaitForSingleObject(q->mutex, INFINITE);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	q->active_threads--;

#ifdef _WIN32
	if (q->active_threads == 0)
		SetEvent(q->done_event);

	ReleaseMutex(q->mutex);
	return 0;
#else
	if (q->active_threads == 0)
		pthread_cond_signal(&q->cond);

	pthread_mutex_unlock(&q->mutex);
	return NULL;
#endif
}

void queue_stop_and_wait(queue_t* q)
{
#ifdef _WIN32
	WaitForSingleObject(q->mutex, INFINITE);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	q->is_running = false;

	int threads_to_wake = q->active_threads;

#ifdef _WIN32
	ReleaseMutex(q->mutex);
#else
	pthread_mutex_unlock(&q->mutex);
#endif

	for (int i = 0; i < threads_to_wake; i++)
	{
#ifdef _WIN32
		ReleaseSemaphore(q->semaphore, 1, NULL);
#else
		sem_post(&q->semaphore);
#endif
	}

#ifdef _WIN32
	WaitForSingleObject(q->done_event, INFINITE);
#else
	pthread_mutex_lock(&q->mutex);
	while (q->active_threads > 0)
		pthread_cond_wait(&q->cond, &q->mutex);
	pthread_mutex_unlock(&q->mutex);
#endif
}

#endif

volatile int variants_count = 0;

#ifdef _WIN32
CRITICAL_SECTION count_mutex;
#else
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void clean_array(int* array, int target, int right_num)
{
	for (int i = 1; i < target - right_num + 1; i++)
		array[i] = 1;

	for (int i = target - right_num + 1; i < target; i++)
		array[i] = 0;

	array[0] = right_num;
}

void print_array(int* tmp, int target)
{
	for (int i = 0; i < target; i++)
	{
		if (tmp[i] != 0)
			printf("%d ", tmp[i]);
	}
	printf("\n");
}

void write_variant(int* buf, int target)
{
#ifdef _WIN32
	EnterCriticalSection(&count_mutex);
#else
	pthread_mutex_lock(&count_mutex);
#endif

	// print_array(buf, target);
	variants_count++;

#ifdef _WIN32
	LeaveCriticalSection(&count_mutex);
#else
	pthread_mutex_unlock(&count_mutex);
#endif
}

typedef struct
{
	int target;
	int right_num;
} count_variants_args_t;

void count_variants(void* arg)
{
	count_variants_args_t* args = (count_variants_args_t*)arg;
	int target = args->target;
	int right_num = args->right_num;

	free(args);

	// Заменяем VLA на vector (работает и на MSVC, и на GCC/Clang)
	std::vector<int> tmp(target + 1);
	std::vector<int> state(target + 1);

	clean_array(tmp.data(), target, right_num);
	write_variant(tmp.data(), target);

	clean_array(state.data(), target, right_num);

	int val_pos = 1;

	while (1)
	{
		state[val_pos]++;

		if (state[val_pos] > state[val_pos - 1])
		{
			state[val_pos] = 1;
			val_pos--;
			if (val_pos == 0)
				break;
			continue;
		}

		int sum = 0;
		for (int i = 0; i <= val_pos; i++)
			sum += state[i];

		int rest = target - sum;

		if (rest < 0)
		{
			state[val_pos] = 1;
			val_pos--;
			if (val_pos == 0)
				break;
			continue;
		}

		for (int i = val_pos + 1; i < target + 1; i++)
			state[i] = (i <= val_pos + rest) ? 1 : 0;

		for (int i = 0; i < target + 1; i++)
			tmp[i] = state[i];

		write_variant(tmp.data(), target);

		if (rest > 0)
			val_pos++;
	}
}

int main()
{
	std::ifstream fin("input.txt");
	std::ofstream fout("output.txt");
	std::ofstream ftime("time.txt");

	int target;
	int threads_count;

	fin >> threads_count;
	fin >> target;

#ifdef _WIN32
	InitializeCriticalSection(&count_mutex);
#endif

	queue_t q;
	queue_init(&q);

#ifdef _WIN32
	std::vector<HANDLE> threads(threads_count);
#else
	std::vector<pthread_t> threads(threads_count);
#endif

	for (int i = 0; i < threads_count; i++)
	{
#ifdef _WIN32
		threads[i] = CreateThread(NULL, 0, worker, &q, 0, NULL);
#else
		pthread_create(&threads[i], NULL, worker, &q);
#endif
	}

	clock_t start = clock();

	for (int i = 1; i < target; i++)
	{
		count_variants_args_t* args =
			(count_variants_args_t*)malloc(sizeof(count_variants_args_t));

		args->target = target;
		args->right_num = i;

		queue_push(&q, count_variants, args);
	}

	queue_stop_and_wait(&q);

	clock_t end = clock();

	for (int i = 0; i < threads_count; i++)
	{
#ifdef _WIN32
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
#else
		pthread_join(threads[i], NULL);
#endif
	}

	queue_destroy(&q);

#ifdef _WIN32
	DeleteCriticalSection(&count_mutex);
#endif

	fout << threads_count << "\n";
	fout << target << "\n";
	fout << variants_count << "\n";

	long ms = (long)(((double)(end - start) / CLOCKS_PER_SEC) * 1000);
	ftime << ms;

	return 0;
}
