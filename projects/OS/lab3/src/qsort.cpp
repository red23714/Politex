#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD 1000

struct Task
{
	int left;
	int right;
};

struct TaskQueue
{
	Task* buf;
	int cap, head, tail, size;
};

static void tq_init(TaskQueue* q, int cap)
{
	q->buf = (Task*)malloc(cap * sizeof(Task));
	q->cap = cap;
	q->head = q->tail = q->size = 0;
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

// ---------- Глобальные ----------

static int* g_arr;
static int g_N, g_T;

static TaskQueue g_queue;
static CRITICAL_SECTION g_cs;
static HANDLE g_sem;

static volatile LONG g_active;
static HANDLE g_done_event;
static volatile int g_terminate = 0;

// ---------- Сортировка ----------

static void insertion_sort(int* a, int l, int r)
{
	for (int i = l + 1; i <= r; i++)
	{
		int key = a[i], j = i - 1;
		while (j >= l && a[j] > key)
		{
			a[j + 1] = a[j];
			j--;
		}
		a[j + 1] = key;
	}
}

static int partition(int* a, int l, int r)
{
	int mid = (l + r) / 2;
	int tmp;

	if (a[l] > a[mid])
	{
		tmp = a[l];
		a[l] = a[mid];
		a[mid] = tmp;
	}
	if (a[l] > a[r])
	{
		tmp = a[l];
		a[l] = a[r];
		a[r] = tmp;
	}
	if (a[mid] > a[r])
	{
		tmp = a[mid];
		a[mid] = a[r];
		a[r] = tmp;
	}

	tmp = a[mid];
	a[mid] = a[r - 1];
	a[r - 1] = tmp;
	int pivot = a[r - 1];

	int i = l, j = r - 1;
	for (;;)
	{
		while (a[++i] < pivot)
		{
		}
		while (a[--j] > pivot)
		{
		}
		if (i >= j)
			break;
		tmp = a[i];
		a[i] = a[j];
		a[j] = tmp;
	}

	tmp = a[i];
	a[i] = a[r - 1];
	a[r - 1] = tmp;
	return i;
}

static void quicksort_seq(int* a, int l, int r)
{
	while (l < r)
	{
		if (r - l + 1 <= 16)
		{
			insertion_sort(a, l, r);
			return;
		}
		int p = partition(a, l, r);
		if (p - l < r - p)
		{
			quicksort_seq(a, l, p - 1);
			l = p + 1;
		}
		else
		{
			quicksort_seq(a, p + 1, r);
			r = p - 1;
		}
	}
}

static void enqueue_task(int l, int r)
{
	Task t = {l, r};

	EnterCriticalSection(&g_cs);
	tq_push(&g_queue, t);
	LeaveCriticalSection(&g_cs);

	ReleaseSemaphore(g_sem, 1, NULL);
}

void process_task(Task task)
{
	int l = task.left;
	int r = task.right;

	if (r - l + 1 <= THRESHOLD)
	{
		quicksort_seq(g_arr, l, r);
	}
	else
	{
		int p = partition(g_arr, l, r);

		int left_ok = (l < p - 1);
		int right_ok = (p + 1 < r);

		if (left_ok)
		{
			InterlockedIncrement(&g_active);
			enqueue_task(l, p - 1);
		}

		if (right_ok)
		{
			Task t = {p + 1, r};
			process_task(t);
		}
	}

	if (InterlockedDecrement(&g_active) == 0)
		SetEvent(g_done_event);
}

DWORD WINAPI worker_thread(void*)
{
	while (1)
	{
		WaitForSingleObject(g_sem, INFINITE);

		if (g_terminate)
			break;

		Task task;
		EnterCriticalSection(&g_cs);
		int ok = tq_pop(&g_queue, &task);
		LeaveCriticalSection(&g_cs);

		if (ok)
			process_task(task);
	}
	return 0;
}

int main()
{
	FILE* fin = fopen("input.txt", "r");
	fscanf(fin, "%d", &g_T);
	fscanf(fin, "%d", &g_N);

	g_arr = (int*)malloc(g_N * sizeof(int));
	for (int i = 0; i < g_N; i++)
		fscanf(fin, "%d", &g_arr[i]);
	fclose(fin);

	InitializeCriticalSection(&g_cs);
	tq_init(&g_queue, 1024);

	g_sem = CreateSemaphore(NULL, 0, 1000000000, NULL);
	g_done_event = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE* threads = (HANDLE*)malloc(g_T * sizeof(HANDLE));
	for (int i = 0; i < g_T; i++)
		threads[i] = CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);

	LARGE_INTEGER f, t1, t2;
	QueryPerformanceFrequency(&f);
	QueryPerformanceCounter(&t1);

	if (g_N > 1)
	{
		g_active = 1;
		enqueue_task(0, g_N - 1);
		WaitForSingleObject(g_done_event, INFINITE);
	}

	QueryPerformanceCounter(&t2);
	long long ms = (t2.QuadPart - t1.QuadPart) * 1000 / f.QuadPart;

	g_terminate = 1;
	ReleaseSemaphore(g_sem, g_T, NULL);

	for (int i = 0; i < g_T; i++)
		WaitForSingleObject(threads[i], INFINITE);

	FILE* fout = fopen("output.txt", "w");
	fprintf(fout, "%d\n%d\n", g_T, g_N);
	for (int i = 0; i < g_N; i++)
	{
		if (i)
			fprintf(fout, " ");
		fprintf(fout, "%d", g_arr[i]);
	}
	fprintf(fout, "\n");
	fclose(fout);

	FILE* ft = fopen("time.txt", "w");
	fprintf(ft, "%lld\n", ms);
	fclose(ft);

	return 0;
}
