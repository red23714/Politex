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

typedef struct
{
	queue_elem* head;
	queue_elem* tail;
	int length;
	int is_running;
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
	q->is_running = 1;
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
	task_t empty = {NULL, NULL};

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

/* =========================================================
   TASKS IN PROGRESS — простой счётчик под мьютексом
   ========================================================= */

typedef struct
{
	int count;
#ifdef _WIN32
	CRITICAL_SECTION mutex;
	CONDITION_VARIABLE cond;
#else
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#endif
} tip_t;

void tip_init(tip_t* tip)
{
#ifdef _WIN32
	InitializeCriticalSection(&tip->mutex);
	InitializeConditionVariable(&tip->cond);
#else
	pthread_mutex_init(&tip->mutex, NULL);
	pthread_cond_init(&tip->cond, NULL);
#endif
	tip->count = 0;
}

void tip_destroy(tip_t* tip)
{
#ifdef _WIN32
	DeleteCriticalSection(&tip->mutex);
#else
	pthread_mutex_destroy(&tip->mutex);
	pthread_cond_destroy(&tip->cond);
#endif
}

void tip_increment(tip_t* tip)
{
#ifdef _WIN32
	EnterCriticalSection(&tip->mutex);
	tip->count++;
	LeaveCriticalSection(&tip->mutex);
#else
	pthread_mutex_lock(&tip->mutex);
	tip->count++;
	pthread_mutex_unlock(&tip->mutex);
#endif
}

/* Декрементируем и будим main, если счётчик дошёл до нуля */
void tip_decrement(tip_t* tip)
{
#ifdef _WIN32
	EnterCriticalSection(&tip->mutex);
	tip->count--;
	if (tip->count == 0)
		WakeConditionVariable(&tip->cond);
	LeaveCriticalSection(&tip->mutex);
#else
	pthread_mutex_lock(&tip->mutex);
	tip->count--;
	if (tip->count == 0)
		pthread_cond_signal(&tip->cond);
	pthread_mutex_unlock(&tip->mutex);
#endif
}

/* Блокируемся до тех пор, пока счётчик не станет нулём */
void tip_wait_zero(tip_t* tip)
{
#ifdef _WIN32
	EnterCriticalSection(&tip->mutex);
	while (tip->count > 0)
		SleepConditionVariableCS(&tip->cond, &tip->mutex, INFINITE);
	LeaveCriticalSection(&tip->mutex);
#else
	pthread_mutex_lock(&tip->mutex);
	while (tip->count > 0)
		pthread_cond_wait(&tip->cond, &tip->mutex);
	pthread_mutex_unlock(&tip->mutex);
#endif
}

/* =========================================================
   WORKER
   ========================================================= */

#ifdef _WIN32
DWORD WINAPI worker(LPVOID arg)
#else
void* worker(void* arg)
#endif
{
	queue_t* q = (queue_t*)arg;

#ifdef _WIN32
	EnterCriticalSection(&q->mutex);
	q->active_threads++;
	LeaveCriticalSection(&q->mutex);
#else
	pthread_mutex_lock(&q->mutex);
	q->active_threads++;
	pthread_mutex_unlock(&q->mutex);
#endif

	while (1)
	{
#ifdef _WIN32
		EnterCriticalSection(&q->mutex);
		int should_stop = !q->is_running && q->length == 0;
		LeaveCriticalSection(&q->mutex);
#else
		pthread_mutex_lock(&q->mutex);
		int should_stop = !q->is_running && q->length == 0;
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
	q->active_threads--;
	if (q->active_threads == 0)
		WakeConditionVariable(&q->cond);
	LeaveCriticalSection(&q->mutex);
	return 0;
#else
	pthread_mutex_lock(&q->mutex);
	q->active_threads--;
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
	q->is_running = 0;
	for (int i = 0; i < q->active_threads; i++)
		ReleaseSemaphore(q->semaphore, 1, NULL);
	while (q->active_threads > 0)
		SleepConditionVariableCS(&q->cond, &q->mutex, INFINITE);
	LeaveCriticalSection(&q->mutex);
#else
	pthread_mutex_lock(&q->mutex);
	q->is_running = 0;
	for (int i = 0; i < q->active_threads; i++)
		sem_post(&q->semaphore);
	while (q->active_threads > 0)
		pthread_cond_wait(&q->cond, &q->mutex);
	pthread_mutex_unlock(&q->mutex);
#endif
}

/* =========================================================
   QUICKSORT
   ========================================================= */

#define THRESHOLD 1000

typedef struct
{
	int* arr;
	int left;
	int right;
	queue_t* q;
	tip_t* tip;
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
	tip_t* tip = data->tip;
	free(data);

	if (left >= right)
	{
		tip_decrement(tip);
		return;
	}

	/* Маленький участок — сортируем последовательно */
	if (right - left <= THRESHOLD)
	{
		quicksort_serial(arr, left, right);
		tip_decrement(tip);
		return;
	}

	/* Разбиение */
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

	/* Отправляем подзадачи в очередь */
	if (left < j)
	{
		task_data* t = (task_data*)malloc(sizeof(task_data));
		t->arr = arr;
		t->left = left;
		t->right = j;
		t->q = q;
		t->tip = tip;
		tip_increment(tip);
		queue_push(q, quicksort_task, t);
	}

	if (i < right)
	{
		task_data* t = (task_data*)malloc(sizeof(task_data));
		t->arr = arr;
		t->left = i;
		t->right = right;
		t->q = q;
		t->tip = tip;
		tip_increment(tip);
		queue_push(q, quicksort_task, t);
	}

	tip_decrement(tip); /* текущая задача завершена */
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

	/* Инициализация */
	queue_t queue;
	queue_init(&queue);

	tip_t tip;
	tip_init(&tip);

	/* Запуск потоков */
#ifdef _WIN32
	HANDLE* threads = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
	for (int i = 0; i < num_threads; i++)
		threads[i] = CreateThread(NULL, 0, worker, &queue, 0, NULL);
#else
	pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
	for (int i = 0; i < num_threads; i++)
		pthread_create(&threads[i], NULL, worker, &queue);
#endif

	/* Первая задача */
	clock_t start = clock();

	task_data* initial = (task_data*)malloc(sizeof(task_data));
	initial->arr = arr;
	initial->left = 0;
	initial->right = N - 1;
	initial->q = &queue;
	initial->tip = &tip;

	tip_increment(&tip);
	queue_push(&queue, quicksort_task, initial);

	/* Ждём завершения сортировки (без busy-wait) */
	tip_wait_zero(&tip);

	clock_t end = clock();

	/* Останавливаем потоки */
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

	/* Запись результатов */
	fprintf(fout, "%d\n%d\n", num_threads, N);
	for (int i = 0; i < N; i++)
		fprintf(fout, "%d ", arr[i]);

	long ms = (long)(((double)(end - start) / CLOCKS_PER_SEC) * 1000);
	fprintf(ftime, "%ld", ms);

	/* Очистка */
	tip_destroy(&tip);
	queue_destroy(&queue);
	free(arr);
	free(threads);
	fclose(fout);
	fclose(ftime);

	return 0;
}
