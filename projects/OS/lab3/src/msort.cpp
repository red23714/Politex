// msort.cpp
// Задание: многопоточная сортировка слияниями массива целых чисел
// ОС: Linux, синхронизация: Mutex + Condition Variable (pthread)
// Компиляция: g++ msort.cpp -D_REENTRANT -lpthread -O2 -o msort
//
// Архитектура:
//   - Пул из T рабочих потоков создаётся один раз до сортировки.
//   - Очередь подзадач Task (диапазон [left, right] массива для сортировки).
//   - Поток берёт подзадачу:
//       * Если диапазон <= THRESHOLD — сортирует рекурсивно (без разбиения).
//       * Иначе — делит пополам, создаёт ДВЕ подзадачи в очереди, и ЖДЁТ их
//       завершения
//         через «барьер слияния» (merge_node), после чего выполняет merge и
//         освобождает результат родительской задаче.
//   - Mutex + Condition Variable:
//       * g_queue_mutex + g_queue_cond : рабочие спят в ожидании задач.
//       * merge_node.mutex + merge_node.cond : поток ждёт завершения двух
//       дочерних задач.
//   - Нет активного ожидания. Нет дедлоков: порядок захвата мьютексов строго
//   определён.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Подмассивы размером <= THRESHOLD сортируются без разбиения на подзадачи
#define THRESHOLD 10000

// ---------- Вспомогательные структуры ----------

// Узел ожидания слияния: родительский поток ждёт завершения двух дочерних задач
struct MergeNode
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	volatile int done_count; // 0, 1 или 2 — сколько дочерних завершились
};

// Подзадача
struct Task
{
	int left;
	int right;
	MergeNode* parent; // узел, которому сообщить о завершении (NULL для корня)
};

// ---------- Очередь подзадач (кольцевой буфер) ----------

struct TaskQueue
{
	Task* buf;
	int cap;
	int head;
	int tail;
	int size;
};

static void tq_init(TaskQueue* q, int cap)
{
	q->buf = (Task*)malloc(cap * sizeof(Task));
	q->cap = cap;
	q->head = 0;
	q->tail = 0;
	q->size = 0;
}

static void tq_push(TaskQueue* q, Task t)
{
	if (q->size == q->cap)
	{
		int newcap = q->cap * 2;
		Task* nb = (Task*)malloc(newcap * sizeof(Task));
		for (int i = 0; i < q->size; i++)
			nb[i] = q->buf[(q->head + i) % q->cap];
		free(q->buf);
		q->buf = nb;
		q->head = 0;
		q->tail = q->size;
		q->cap = newcap;
	}
	q->buf[q->tail] = t;
	q->tail = (q->tail + 1) % q->cap;
	q->size++;
}

static int tq_pop(TaskQueue* q, Task* out)
{
	if (q->size == 0)
		return 0;
	*out = q->buf[q->head];
	q->head = (q->head + 1) % q->cap;
	q->size--;
	return 1;
}

static void tq_free(TaskQueue* q)
{
	free(q->buf);
	q->buf = NULL;
}

// ---------- Глобальные данные ----------

static int* g_arr; // Сортируемый массив
static int* g_tmp; // Вспомогательный буфер для слияния
static int g_N;
static int g_T;

static TaskQueue g_queue;
static pthread_mutex_t g_queue_mutex;
static pthread_cond_t g_queue_cond; // Сигнал: в очереди появилась задача
static volatile int g_active;		// Количество незавершённых задач
static volatile int g_terminate;	// Флаг завершения для рабочих

// Событие «все задачи завершены» для главного потока
static pthread_mutex_t g_done_mutex;
static pthread_cond_t g_done_cond;

// ---------- Merge ----------

static void merge(int* a, int* tmp, int left, int mid, int right)
{
	int i = left, j = mid + 1, k = left;
	while (i <= mid && j <= right)
	{
		if (a[i] <= a[j])
			tmp[k++] = a[i++];
		else
			tmp[k++] = a[j++];
	}
	while (i <= mid)
		tmp[k++] = a[i++];
	while (j <= right)
		tmp[k++] = a[j++];
	memcpy(a + left, tmp + left, (right - left + 1) * sizeof(int));
}

// ---------- Рекурсивная однопоточная сортировка слияниями ----------

static void mergesort_seq(int* a, int* tmp, int left, int right)
{
	if (left >= right)
		return;
	int mid = left + (right - left) / 2;
	mergesort_seq(a, tmp, left, mid);
	mergesort_seq(a, tmp, mid + 1, right);
	merge(a, tmp, left, mid, right);
}

// ---------- Добавление задачи в очередь ----------

static void enqueue(Task t)
{
	pthread_mutex_lock(&g_queue_mutex);
	tq_push(&g_queue, t);
	pthread_cond_signal(&g_queue_cond); // Будим один рабочий поток
	pthread_mutex_unlock(&g_queue_mutex);
}

// ---------- Поточная функция ----------

void* worker_thread(void* param)
{
	while (1)
	{
		// Ждём задачи в очереди
		pthread_mutex_lock(&g_queue_mutex);
		while (g_queue.size == 0 && !g_terminate)
			pthread_cond_wait(&g_queue_cond, &g_queue_mutex);

		if (g_terminate && g_queue.size == 0)
		{
			pthread_mutex_unlock(&g_queue_mutex);
			break;
		}

		Task task;
		tq_pop(&g_queue, &task);
		pthread_mutex_unlock(&g_queue_mutex);

		int left = task.left;
		int right = task.right;

		if (right - left + 1 <= THRESHOLD)
		{
			// Малый диапазон — сортируем без разбиения
			mergesort_seq(g_arr, g_tmp, left, right);
		}
		else
		{
			int mid = left + (right - left) / 2;

			// Создаём узел ожидания слияния
			MergeNode node;
			pthread_mutex_init(&node.mutex, NULL);
			pthread_cond_init(&node.cond, NULL);
			node.done_count = 0;

			// Атомарно добавляем 2 дочерние задачи к счётчику активных
			// (текущая задача ещё не завершена — она «превращается» в
			// ожидающую)
			__sync_add_and_fetch(&g_active, 2);

			// Размещаем дочерние задачи в очереди
			Task left_task;
			left_task.left = left;
			left_task.right = mid;
			left_task.parent = &node;

			Task right_task;
			right_task.left = mid + 1;
			right_task.right = right;
			right_task.parent = &node;

			// Добавляем обе задачи в очередь (будим два потока)
			pthread_mutex_lock(&g_queue_mutex);
			tq_push(&g_queue, left_task);
			tq_push(&g_queue, right_task);
			pthread_cond_signal(&g_queue_cond);
			pthread_cond_signal(&g_queue_cond);
			pthread_mutex_unlock(&g_queue_mutex);

			// Ждём завершения обеих дочерних задач
			pthread_mutex_lock(&node.mutex);
			while (node.done_count < 2)
				pthread_cond_wait(&node.cond, &node.mutex);
			pthread_mutex_unlock(&node.mutex);

			// Обе части отсортированы — выполняем слияние
			merge(g_arr, g_tmp, left, mid, right);

			pthread_mutex_destroy(&node.mutex);
			pthread_cond_destroy(&node.cond);
		}

		// Уведомляем родителя о завершении текущей задачи
		if (task.parent != NULL)
		{
			pthread_mutex_lock(&task.parent->mutex);
			task.parent->done_count++;
			if (task.parent->done_count == 2)
				pthread_cond_signal(&task.parent->cond);
			pthread_mutex_unlock(&task.parent->mutex);
		}

		// Уменьшаем счётчик активных задач
		int remaining = __sync_sub_and_fetch(&g_active, 1);
		if (remaining == 0)
		{
			// Все задачи завершены — будим главный поток
			pthread_mutex_lock(&g_done_mutex);
			pthread_cond_signal(&g_done_cond);
			pthread_mutex_unlock(&g_done_mutex);
		}
	}

	return NULL;
}

// ---------- Замер времени в мс ----------

static long long timespec_ms(struct timespec* t)
{
	return (long long)t->tv_sec * 1000LL + (long long)t->tv_nsec / 1000000LL;
}

// ---------- Основная функция ----------

int main()
{
	// --- Чтение входных данных ---
	FILE* fin = fopen("input.txt", "r");
	if (!fin)
	{
		fprintf(stderr, "Cannot open input.txt\n");
		return 1;
	}
	fscanf(fin, "%d", &g_T);
	fscanf(fin, "%d", &g_N);
	g_arr = (int*)malloc(g_N * sizeof(int));
	g_tmp = (int*)malloc(g_N * sizeof(int));
	if (!g_arr || !g_tmp)
	{
		fprintf(stderr, "Out of memory\n");
		return 1;
	}
	for (int i = 0; i < g_N; i++)
		fscanf(fin, "%d", &g_arr[i]);
	fclose(fin);

	// --- Инициализация ---
	tq_init(&g_queue, 1024);
	pthread_mutex_init(&g_queue_mutex, NULL);
	pthread_cond_init(&g_queue_cond, NULL);
	pthread_mutex_init(&g_done_mutex, NULL);
	pthread_cond_init(&g_done_cond, NULL);
	g_active = 0;
	g_terminate = 0;

	// --- Создание пула потоков ---
	pthread_t* threads = (pthread_t*)malloc(g_T * sizeof(pthread_t));
	for (int t = 0; t < g_T; t++)
		pthread_create(&threads[t], NULL, worker_thread, NULL);

	// --- Замер времени и запуск ---
	struct timespec t_start, t_end;
	clock_gettime(CLOCK_REALTIME, &t_start);

	if (g_N > 1)
	{
		// Добавляем корневую задачу
		__sync_add_and_fetch(&g_active, 1);

		Task root;
		root.left = 0;
		root.right = g_N - 1;
		root.parent = NULL;
		enqueue(root);

		// Ждём завершения всех задач
		pthread_mutex_lock(&g_done_mutex);
		while (g_active > 0)
			pthread_cond_wait(&g_done_cond, &g_done_mutex);
		pthread_mutex_unlock(&g_done_mutex);
	}

	clock_gettime(CLOCK_REALTIME, &t_end);
	long long elapsed_ms = timespec_ms(&t_end) - timespec_ms(&t_start);

	// --- Завершение потоков ---
	g_terminate = 1;
	pthread_mutex_lock(&g_queue_mutex);
	pthread_cond_broadcast(&g_queue_cond); // Будим все потоки
	pthread_mutex_unlock(&g_queue_mutex);
	for (int t = 0; t < g_T; t++)
		pthread_join(threads[t], NULL);

	// --- Запись output.txt ---
	FILE* fout = fopen("output.txt", "w");
	if (!fout)
	{
		fprintf(stderr, "Cannot open output.txt\n");
		return 1;
	}
	fprintf(fout, "%d\n%d\n", g_T, g_N);
	for (int i = 0; i < g_N; i++)
	{
		if (i > 0)
			fprintf(fout, " ");
		fprintf(fout, "%d", g_arr[i]);
	}
	fprintf(fout, "\n");
	fclose(fout);

	// --- Запись time.txt ---
	FILE* ftime = fopen("time.txt", "w");
	if (!ftime)
	{
		fprintf(stderr, "Cannot open time.txt\n");
		return 1;
	}
	fprintf(ftime, "%lld\n", elapsed_ms);
	fclose(ftime);

	// --- Освобождение ресурсов ---
	free(g_arr);
	free(g_tmp);
	free(threads);
	tq_free(&g_queue);
	pthread_mutex_destroy(&g_queue_mutex);
	pthread_cond_destroy(&g_queue_cond);
	pthread_mutex_destroy(&g_done_mutex);
	pthread_cond_destroy(&g_done_cond);

	return 0;
}
