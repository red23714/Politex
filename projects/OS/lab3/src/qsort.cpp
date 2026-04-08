// qsort.cpp
// Задание: многопоточная быстрая сортировка массива целых чисел
// ОС: Windows, синхронизация: Semaphore + Critical Section
// Компилятор: Visual Studio 2010
//
// Архитектура:
//   - Пул из T рабочих потоков создаётся один раз до сортировки.
//   - Есть очередь подзадач (Task). Каждая подзадача — диапазон [left, right]
//   массива.
//   - Поток берёт подзадачу, выполняет partition, затем:
//       * Если подмассив <= THRESHOLD элементов — сортирует рекурсивно без
//       разбиения.
//       * Иначе — помещает две подзадачи (левую и правую части) обратно в
//       очередь.
//   - Семафор: рабочие потоки блокируются на нём в ожидании задач (нет
//   активного ожидания).
//     При добавлении задачи в очередь семафор освобождается (ReleaseSemaphore).
//   - Критическая секция: защищает очередь задач и счётчик активных задач.
//   - Завершение: когда счётчик активных задач достигает 0, основной поток
//   получает сигнал.
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Подмассивы размером <= THRESHOLD сортируются рекурсивно, без помещения в
// очередь
#define THRESHOLD 1000

// ---------- Очередь подзадач ----------

struct Task
{
	int left;
	int right;
};

// Простая очередь на основе динамического кольцевого буфера
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
		// Расширяем буфер вдвое
		int newcap = q->cap * 2;
		Task* newbuf = (Task*)malloc(newcap * sizeof(Task));
		for (int i = 0; i < q->size; i++)
			newbuf[i] = q->buf[(q->head + i) % q->cap];
		free(q->buf);
		q->buf = newbuf;
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
static int g_N;	   // Размер массива
static int g_T;	   // Количество потоков пула

static TaskQueue g_queue;	  // Очередь подзадач
static CRITICAL_SECTION g_cs; // Защита очереди и счётчика
static HANDLE g_sem;		  // Семафор: кол-во доступных задач
static volatile LONG
	g_active; // Количество «живых» подзадач (в очереди + в обработке)
static HANDLE
	g_done_event; // Event (auto-reset): сигнал основному потоку что всё готово
static volatile int g_terminate; // Флаг завершения для рабочих потоков

// ---------- QuickSort вспомогательные функции ----------

// Сортировка вставками для маленьких диапазонов
static void insertion_sort(int* a, int left, int right)
{
	for (int i = left + 1; i <= right; i++)
	{
		int key = a[i];
		int j = i - 1;
		while (j >= left && a[j] > key)
		{
			a[j + 1] = a[j];
			j--;
		}
		a[j + 1] = key;
	}
}

// Выбор pivot методом медианы трёх
static int median_of_three(int* a, int left, int right)
{
	int mid = left + (right - left) / 2;
	if (a[left] > a[mid])
	{
		int t = a[left];
		a[left] = a[mid];
		a[mid] = t;
	}
	if (a[left] > a[right])
	{
		int t = a[left];
		a[left] = a[right];
		a[right] = t;
	}
	if (a[mid] > a[right])
	{
		int t = a[mid];
		a[mid] = a[right];
		a[right] = t;
	}
	// Теперь a[mid] — медиана; помещаем pivot в right-1
	int t = a[mid];
	a[mid] = a[right - 1];
	a[right - 1] = t;
	return a[right - 1];
}

// Partition, возвращает индекс pivot после разбиения
static int partition(int* a, int left, int right)
{
	if (right - left < 2)
	{
		if (a[left] > a[right])
		{
			int t = a[left];
			a[left] = a[right];
			a[right] = t;
		}
		return left;
	}
	int pivot = median_of_three(a, left, right);
	int i = left;
	int j = right - 1;
	while (1)
	{
		while (a[++i] < pivot)
		{
		}
		while (a[--j] > pivot)
		{
		}
		if (i >= j)
			break;
		int t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
	// Возвращаем pivot на место
	int t = a[i];
	a[i] = a[right - 1];
	a[right - 1] = t;
	return i;
}

// Рекурсивная (однопоточная) быстрая сортировка для малых диапазонов
static void quicksort_seq(int* a, int left, int right)
{
	while (left < right)
	{
		if (right - left < 16)
		{
			insertion_sort(a, left, right);
			return;
		}
		int p = partition(a, left, right);
		// «Хвостовая рекурсия»: рекурсируем в меньшую часть, итерируемся в
		// большую
		if (p - left < right - p)
		{
			quicksort_seq(a, left, p - 1);
			left = p + 1;
		}
		else
		{
			quicksort_seq(a, p + 1, right);
			right = p - 1;
		}
	}
}

// ---------- Добавление задачи в очередь ----------

// Вызывается только когда уже известно, что задача «живая» (g_active уже
// увеличен)
static void enqueue_task(int left, int right)
{
	Task t;
	t.left = left;
	t.right = right;

	EnterCriticalSection(&g_cs);
	tq_push(&g_queue, t);
	LeaveCriticalSection(&g_cs);

	// Разблокировать один рабочий поток
	ReleaseSemaphore(g_sem, 1, NULL);
}

// ---------- Поточная функция ----------

DWORD WINAPI worker_thread(void* param)
{
	while (1)
	{
		// Ждём появления задачи (блокирующее ожидание, без активного цикла)
		WaitForSingleObject(g_sem, INFINITE);

		// Проверяем флаг завершения
		if (g_terminate)
			break;

		// Извлекаем задачу из очереди
		Task task;
		EnterCriticalSection(&g_cs);
		int got = tq_pop(&g_queue, &task);
		LeaveCriticalSection(&g_cs);

		if (!got)
		{
			// Семафор был освобождён из-за завершения — выходим
			if (g_terminate)
				break;
			continue;
		}

		int left = task.left;
		int right = task.right;

		if (right - left + 1 <= THRESHOLD)
		{
			// Малый диапазон — сортируем рекурсивно без разбиения
			quicksort_seq(g_arr, left, right);

			// Задача выполнена
			LONG remaining = InterlockedDecrement(&g_active);
			if (remaining == 0)
				SetEvent(g_done_event);
		}
		else
		{
			// Большой диапазон: разбиваем на две подзадачи
			int p = partition(g_arr, left, right);

			// Сначала увеличиваем счётчик на 2 (новые подзадачи),
			// затем уменьшаем на 1 (текущая завершена) = net +1
			InterlockedAdd(&g_active, 1); // +2 - 1 = +1

			// Добавляем две подзадачи в очередь
			if (p - 1 >= left)
				enqueue_task(left, p - 1);
			else
			{
				// Левая часть пуста — сразу засчитываем как выполненную
				LONG rem = InterlockedDecrement(&g_active);
				if (rem == 0)
					SetEvent(g_done_event);
			}

			if (p + 1 <= right)
				enqueue_task(p + 1, right);
			else
			{
				LONG rem = InterlockedDecrement(&g_active);
				if (rem == 0)
					SetEvent(g_done_event);
			}
		}
	}

	return 0;
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
	if (!g_arr)
	{
		fprintf(stderr, "Out of memory\n");
		return 1;
	}
	for (int i = 0; i < g_N; i++)
		fscanf(fin, "%d", &g_arr[i]);
	fclose(fin);

	// --- Инициализация объектов синхронизации ---
	InitializeCriticalSection(&g_cs);
	tq_init(&g_queue, 1024);

	// Семафор: начальное значение 0, максимальное — большое число
	g_sem = CreateSemaphore(NULL, 0, g_N + g_T + 1024, NULL);
	g_done_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_terminate = 0;
	g_active = 0;

	// --- Создание пула потоков ---
	HANDLE* threads = (HANDLE*)malloc(g_T * sizeof(HANDLE));
	for (int t = 0; t < g_T; t++)
		threads[t] =
			CreateThread(NULL, 0, worker_thread, (char*)0 + t, 0, NULL);

	// --- Замер времени ---
	LARGE_INTEGER freq, t_start, t_end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t_start);

	// --- Запуск сортировки ---
	if (g_N > 1)
	{
		// Добавляем первую задачу
		InterlockedExchange(&g_active, 1);
		enqueue_task(0, g_N - 1);

		// Ждём завершения всех подзадач
		WaitForSingleObject(g_done_event, INFINITE);
	}

	QueryPerformanceCounter(&t_end);
	long long elapsed_ms = (long long)((t_end.QuadPart - t_start.QuadPart) *
									   1000LL / freq.QuadPart);

	// --- Завершение потоков ---
	g_terminate = 1;
	// Освобождаем семафор g_T раз, чтобы разбудить все потоки
	ReleaseSemaphore(g_sem, g_T, NULL);
	WaitForMultipleObjects(g_T, threads, TRUE, INFINITE);

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
	tq_free(&g_queue);
	DeleteCriticalSection(&g_cs);
	CloseHandle(g_sem);
	CloseHandle(g_done_event);
	for (int t = 0; t < g_T; t++)
		CloseHandle(threads[t]);
	free(threads);

	return 0;
}
