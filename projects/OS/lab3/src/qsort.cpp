#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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

#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
using namespace std;

const int THRESHOLD = 1000;

struct task_data
{
	int* arr;
	int left;
	int right;
	queue_t* q;
};

atomic<int> tasks_in_progress(0);

void quicksort_serial(int* arr, int left, int right)
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
			swap(arr[i], arr[j]);
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
		delete data;
		tasks_in_progress--;
		return;
	}

	if (right - left <= THRESHOLD)
	{
		quicksort_serial(arr, left, right);
		delete data;
		tasks_in_progress--;
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
			swap(arr[i], arr[j]);
			i++;
			j--;
		}
	}

	if (left < j)
	{
		task_data* left_task = new task_data{arr, left, j, q};
		tasks_in_progress++;
		queue_push(q, quicksort_task, left_task);
	}

	if (i < right)
	{
		task_data* right_task = new task_data{arr, i, right, q};
		tasks_in_progress++;
		queue_push(q, quicksort_task, right_task);
	}

	delete data;
	tasks_in_progress--;
}

int main()
{
	ifstream fin("input.txt");
	ofstream fout("output.txt");
	ofstream ftime("time.txt");

	int num_threads;
	int N;

	fin >> num_threads;
	fin >> N;

	vector<int> arr(N);
	for (int i = 0; i < N; i++)
		fin >> arr[i];

	queue_t queue;
	queue_init(&queue);

#ifdef _WIN32
	using thread_t = HANDLE;
#else
	using thread_t = pthread_t;
#endif

	vector<thread_t> threads(num_threads);

	for (int i = 0; i < num_threads; i++)
	{
#ifdef _WIN32
		threads[i] = CreateThread(NULL, 0, worker, &queue, 0, NULL);
#else
		pthread_create(&threads[i], NULL, worker, &queue);
#endif
	}

	auto start = chrono::high_resolution_clock::now();

	task_data* initial = new task_data{arr.data(), 0, N - 1, &queue};
	tasks_in_progress = 1;
	queue_push(&queue, quicksort_task, initial);

	while (tasks_in_progress > 0)
	{
		this_thread::yield();
	}

	auto end = chrono::high_resolution_clock::now();

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

	fout << num_threads << "\n";
	fout << N << "\n";

	for (int i = 0; i < N; i++)
		fout << arr[i] << " ";

	long long ms =
		chrono::duration_cast<chrono::milliseconds>(end - start).count();
	ftime << ms;

	queue_destroy(&queue);

	return 0;
}
