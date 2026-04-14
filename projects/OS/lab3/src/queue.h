#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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
	bool is_running;	// Флаг работы очереди
	int active_threads; // Счётчик активных потоков

	pthread_mutex_t mutex;
	pthread_cond_t cond; // Для ожидания завершения
	sem_t semaphore;	 // Семафор для заданий
} queue_t;

// Инициализация очереди
int queue_init(queue_t* q)
{
	if (!q)
		return -1;

	q->head = NULL;
	q->tail = NULL;
	q->length = 0;
	q->is_running = true;
	q->active_threads = 0;

	// Инициализация мьютекса
	if (pthread_mutex_init(&q->mutex, NULL) != 0)
	{
		perror("pthread_mutex_init");
		return -1;
	}

	// Инициализация condition variable
	if (pthread_cond_init(&q->cond, NULL) != 0)
	{
		perror("pthread_cond_init");
		pthread_mutex_destroy(&q->mutex);
		return -1;
	}

	// Инициализация семафора (начальное значение 0 - заданий нет)
	if (sem_init(&q->semaphore, 0, 0) != 0)
	{
		perror("sem_init");
		pthread_mutex_destroy(&q->mutex);
		pthread_cond_destroy(&q->cond);
		return -1;
	}

	return 0;
}

// Уничтожение очереди
void queue_destroy(queue_t* q)
{
	if (!q)
		return;

	pthread_mutex_lock(&q->mutex);

	// Очистка оставшихся заданий
	while (q->head != NULL)
	{
		queue_elem* tmp = q->head;
		q->head = q->head->next;
		free(tmp);
	}

	pthread_mutex_unlock(&q->mutex);

	pthread_mutex_destroy(&q->mutex);
	pthread_cond_destroy(&q->cond);
	sem_destroy(&q->semaphore);
}

// Взятие задания из очереди (блокирующее)
task_t queue_pop(queue_t* q)
{
	// Ждём, пока в семафоре появится хотя бы одно задание
	sem_wait(&q->semaphore);

	pthread_mutex_lock(&q->mutex);

	if (q->head == NULL)
	{
		pthread_mutex_unlock(&q->mutex);
		task_t empty = {NULL, NULL};
		return empty;
	}

	task_t t = q->head->task;
	queue_elem* tmp = q->head;
	q->head = q->head->next;
	if (q->head == NULL)
	{
		q->tail = NULL;
	}
	q->length--;
	free(tmp);

	pthread_mutex_unlock(&q->mutex);

	return t;
}

// Добавление задания в очередь
void queue_push(queue_t* q, task_func_t func, void* arg)
{
	queue_elem* tmp = (queue_elem*)malloc(sizeof(queue_elem));
	if (!tmp)
	{
		fprintf(stderr, "Failed to allocate memory for queue element\n");
		return;
	}

	tmp->task.func = func;
	tmp->task.arg = arg;
	tmp->next = NULL;

	pthread_mutex_lock(&q->mutex);

	if (!q->is_running)
	{
		free(tmp);
		pthread_mutex_unlock(&q->mutex);
		return;
	}

	if (q->tail == NULL)
	{
		q->head = q->tail = tmp;
	}
	else
	{
		q->tail->next = tmp;
		q->tail = tmp;
	}
	q->length++;

	pthread_mutex_unlock(&q->mutex);

	// Увеличиваем счётчик семафора (появилось новое задание)
	sem_post(&q->semaphore);
}

// Функция потока-воркера
void* worker(void* arg)
{
	queue_t* q = (queue_t*)arg;

	pthread_mutex_lock(&q->mutex);
	q->active_threads++;
	pthread_mutex_unlock(&q->mutex);

	while (1)
	{
		// Проверяем, нужно ли завершать работу
		pthread_mutex_lock(&q->mutex);
		bool should_stop = !q->is_running && q->length == 0;
		pthread_mutex_unlock(&q->mutex);

		if (should_stop)
		{
			break;
		}

		task_t t = queue_pop(q);
		if (t.func != NULL)
		{
			t.func(t.arg);
		}
	}

	// Поток завершается
	pthread_mutex_lock(&q->mutex);
	q->active_threads--;
	if (q->active_threads == 0)
	{
		pthread_cond_signal(&q->cond); // Сигналим, что все потоки завершились
	}
	pthread_mutex_unlock(&q->mutex);

	return NULL;
}

// Остановка очереди и ожидание завершения всех потоков
void queue_stop_and_wait(queue_t* q)
{
	pthread_mutex_lock(&q->mutex);
	q->is_running = false; // Больше не принимаем новые задания

	// Разбудить все потоки, которые ждут на семафоре
	// Отправляем "пустые" сигналы для каждого активного потока
	int threads_to_wake = q->active_threads;
	for (int i = 0; i < threads_to_wake; i++)
	{
		sem_post(&q->semaphore);
	}

	// Ждём, пока все потоки завершатся
	while (q->active_threads > 0)
	{
		pthread_cond_wait(&q->cond, &q->mutex);
	}

	pthread_mutex_unlock(&q->mutex);
}

#endif
