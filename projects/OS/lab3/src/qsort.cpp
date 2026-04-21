#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#endif

/* =========================================================
   QUEUE
   ========================================================= */

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
	CRITICAL_SECTION mutex;
	CONDITION_VARIABLE cond;
	HANDLE semaphore;
#else
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	sem_t semaphore;
#endif
} queue_t;

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
	InitializeCriticalSection(&q->mutex);
	InitializeConditionVariable(&q->cond);
	q->semaphore = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	if (!q->semaphore)
		return -1;
#else
	if (pthread_mutex_init(&q->mutex, NULL) != 0)
		return -1;
	if (pthread_cond_init(&q->cond, NULL) != 0)
		return -1;
	if (sem_init(&q->semaphore, 0, 0) != 0)
		return -1;
#endif

	return 0;
}

void queue_destroy(queue_t* q)
{
#ifdef _WIN32
	EnterCriticalSection(&q->mutex);
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
	LeaveCriticalSection(&q->mutex);
	DeleteCriticalSection(&q->mutex);
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
	EnterCriticalSection(&q->mutex);
#else
	sem_wait(&q->semaphore);
	pthread_mutex_lock(&q->mutex);
#endif

	if (!q->head)
	{
#ifdef _WIN32
		LeaveCriticalSection(&q->mutex);
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
	LeaveCriticalSection(&q->mutex);
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
	EnterCriticalSection(&q->mutex);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	if (!q->is_running)
	{
		free(tmp);
#ifdef _WIN32
		LeaveCriticalSection(&q->mutex);
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
	LeaveCriticalSection(&q->mutex);
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
	EnterCriticalSection(&q->mutex);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	q->active_threads++;

#ifdef _WIN32
	LeaveCriticalSection(&q->mutex);
#else
	pthread_mutex_unlock(&q->mutex);
#endif

	while (1)
	{
#ifdef _WIN32
		EnterCriticalSection(&q->mutex);
#else
		pthread_mutex_lock(&q->mutex);
#endif

		bool should_stop = !q->is_running && q->length == 0;

#ifdef _WIN32
		LeaveCriticalSection(&q->mutex);
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
	EnterCriticalSection(&q->mutex);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	q->active_threads--;

#ifdef _WIN32
	if (q->active_threads == 0)
		WakeConditionVariable(&q->cond);
	LeaveCriticalSection(&q->mutex);
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
	EnterCriticalSection(&q->mutex);
#else
	pthread_mutex_lock(&q->mutex);
#endif

	q->is_running = false;

	int threads_to_wake = q->active_threads;
	for (int i = 0; i < threads_to_wake; i++)
	{
#ifdef _WIN32
		ReleaseSemaphore(q->semaphore, 1, NULL);
#else
		sem_post(&q->semaphore);
#endif
	}

	while (q->active_threads > 0)
	{
#ifdef _WIN32
		SleepConditionVariableCS(&q->cond, &q->mutex, INFINITE);
#else
		pthread_cond_wait(&q->cond, &q->mutex);
#endif
	}

#ifdef _WIN32
	LeaveCriticalSection(&q->mutex);
#else
	pthread_mutex_unlock(&q->mutex);
#endif
}

/* =========================================================
   QUICKSORT
   ========================================================= */

#define THRESHOLD 1000

/* Атомарный счётчик задач в работе */
#ifdef _WIN32
static volatile LONG tasks_in_progress = 0;
#else
static volatile int tasks_in_progress = 0;
static pthread_mutex_t tip_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static void tip_increment(void)
{
#ifdef _WIN32
	InterlockedIncrement(&tasks_in_progress);
#else
	__sync_fetch_and_add(&tasks_in_progress, 1);
#endif
}

static void tip_decrement(void)
{
#ifdef _WIN32
	InterlockedDecrement(&tasks_in_progress);
#else
	__sync_fetch_and_add(&tasks_in_progress, -1);
#endif
}

static int tip_load(void)
{
#ifdef _WIN32
	return (int)InterlockedCompareExchange(&tasks_in_progress, 0, 0);
#else
	return __sync_fetch_and_add(&tasks_in_progress, 0);
#endif
}

typedef struct
{
	int* arr;
	int left;
	int right;
	queue_t* q;
} task_data;

static void swap_int(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

static void quicksort_serial(int* arr, int left, int right)
{
	if (left >= right)
		return;

	int i = left, j = right;
	int pivot = arr[(left + right) / 2];

	while (i <= j)
	{
		while (arr[i] < pivot)
			i++;
		while (arr[j] > pivot)
			j--;

		if (i <= j)
		{
			swap_int(&arr[i], &arr[j]);
			i++;
			j--;
		}
	}

	if (left < j)
		quicksort_serial(arr, left, j);
	if (i < right)
		quicksort_serial(arr, i, right);
}

void quicksort_task(void* arg)
{
	task_data* data = (task_data*)arg;

	int left = data->left;
	int right = data->right;
	int* arr = data->arr;
	queue_t* q = data->q;

	if (left >= right)
	{
		free(data);
		tip_decrement();
		return;
	}

	if (right - left <= THRESHOLD)
	{
		quicksort_serial(arr, left, right);
		free(data);
		tip_decrement();
		return;
	}

	int i = left, j = right;
	int pivot = arr[(left + right) / 2];

	while (i <= j)
	{
		while (arr[i] < pivot)
			i++;
		while (arr[j] > pivot)
			j--;

		if (i <= j)
		{
			swap_int(&arr[i], &arr[j]);
			i++;
			j--;
		}
	}

	if (left < j)
	{
		task_data* left_task = (task_data*)malloc(sizeof(task_data));
		left_task->arr = arr;
		left_task->left = left;
		left_task->right = j;
		left_task->q = q;
		tip_increment();
		queue_push(q, quicksort_task, left_task);
	}

	if (i < right)
	{
		task_data* right_task = (task_data*)malloc(sizeof(task_data));
		right_task->arr = arr;
		right_task->left = i;
		right_task->right = right;
		right_task->q = q;
		tip_increment();
		queue_push(q, quicksort_task, right_task);
	}

	free(data);
	tip_decrement();
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
	FILE* fin = fopen("input.txt", "r");
	FILE* fout = fopen("output.txt", "w");
	FILE* ftime = fopen("time.txt", "w");

	int num_threads, N;
	fscanf(fin, "%d", &num_threads);
	fscanf(fin, "%d", &N);

	int* arr = (int*)malloc(N * sizeof(int));
	for (int i = 0; i < N; i++)
		fscanf(fin, "%d", &arr[i]);

	fclose(fin);

	queue_t queue;
	queue_init(&queue);

#ifdef _WIN32
	HANDLE* threads = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
#else
	pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
#endif

	for (int i = 0; i < num_threads; i++)
	{
#ifdef _WIN32
		threads[i] = CreateThread(NULL, 0, worker, &queue, 0, NULL);
#else
		pthread_create(&threads[i], NULL, worker, &queue);
#endif
	}

	clock_t start = clock();

	task_data* initial = (task_data*)malloc(sizeof(task_data));
	initial->arr = arr;
	initial->left = 0;
	initial->right = N - 1;
	initial->q = &queue;

	tip_increment();
	queue_push(&queue, quicksort_task, initial);

	while (tip_load() > 0)
	{
#ifdef _WIN32
		Sleep(0);
#else
		sched_yield();
#endif
	}

	clock_t end = clock();

	queue_stop_and_wait(&queue);

	for (int i = 0; i < num_threads; i++)
	{
#ifdef _WIN32
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
#else
		pthread_join(threads[i], NULL);
#endif
	}

	fprintf(fout, "%d\n", num_threads);
	fprintf(fout, "%d\n", N);
	for (int i = 0; i < N; i++)
		fprintf(fout, "%d ", arr[i]);

	long ms = (long)(((double)(end - start) / CLOCKS_PER_SEC) * 1000);
	fprintf(ftime, "%ld", ms);

	queue_destroy(&queue);
	free(arr);
	free(threads);

	fclose(fout);
	fclose(ftime);

	return 0;
}
