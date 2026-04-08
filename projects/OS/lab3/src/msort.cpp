#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <atomic>

#define THRESHOLD 100000

struct Task
{
	int left;
	int right;
};

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

static int* g_arr;
static int* g_tmp;
static int g_N;
static int g_T;

static TaskQueue g_queue;
static pthread_mutex_t g_queue_mutex;
static pthread_cond_t g_queue_cond;

static std::atomic<int> g_active;
static int g_terminate = 0;

static pthread_mutex_t g_done_mutex;
static pthread_cond_t g_done_cond;

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

static void mergesort_seq(int* a, int* tmp, int left, int right)
{
	if (left >= right)
		return;
	int mid = (left + right) / 2;
	mergesort_seq(a, tmp, left, mid);
	mergesort_seq(a, tmp, mid + 1, right);
	merge(a, tmp, left, mid, right);
}

static void enqueue(Task t)
{
	pthread_mutex_lock(&g_queue_mutex);
	tq_push(&g_queue, t);
	pthread_cond_signal(&g_queue_cond);
	pthread_mutex_unlock(&g_queue_mutex);
}

void process_task(Task task)
{
	int left = task.left;
	int right = task.right;

	if (right - left + 1 <= THRESHOLD)
	{
		mergesort_seq(g_arr, g_tmp, left, right);
	}
	else
	{
		int mid = (left + right) / 2;

		Task left_task = {left, mid};
		Task right_task = {mid + 1, right};

		g_active.fetch_add(1);
		enqueue(left_task);

		process_task(right_task);

		merge(g_arr, g_tmp, left, mid, right);
	}

	if (g_active.fetch_sub(1) == 1)
	{
		pthread_mutex_lock(&g_done_mutex);
		pthread_cond_signal(&g_done_cond);
		pthread_mutex_unlock(&g_done_mutex);
	}
}

void* worker_thread(void*)
{
	while (1)
	{
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

		process_task(task);
	}
	return NULL;
}

static long long timespec_ms(struct timespec* t)
{
	return (long long)t->tv_sec * 1000LL + t->tv_nsec / 1000000LL;
}

int main()
{
	FILE* fin = fopen("input.txt", "r");
	fscanf(fin, "%d", &g_T);
	fscanf(fin, "%d", &g_N);

	g_arr = (int*)malloc(g_N * sizeof(int));
	g_tmp = (int*)malloc(g_N * sizeof(int));

	for (int i = 0; i < g_N; i++)
		fscanf(fin, "%d", &g_arr[i]);
	fclose(fin);

	tq_init(&g_queue, 1024);
	pthread_mutex_init(&g_queue_mutex, NULL);
	pthread_cond_init(&g_queue_cond, NULL);
	pthread_mutex_init(&g_done_mutex, NULL);
	pthread_cond_init(&g_done_cond, NULL);

	g_active = 0;

	pthread_t* threads = (pthread_t*)malloc(g_T * sizeof(pthread_t));
	for (int i = 0; i < g_T; i++)
		pthread_create(&threads[i], NULL, worker_thread, NULL);

	struct timespec t1, t2;
	clock_gettime(CLOCK_REALTIME, &t1);

	if (g_N > 1)
	{
		g_active = 1;
		Task root = {0, g_N - 1};
		enqueue(root);

		pthread_mutex_lock(&g_done_mutex);
		while (g_active > 0)
			pthread_cond_wait(&g_done_cond, &g_done_mutex);
		pthread_mutex_unlock(&g_done_mutex);
	}

	clock_gettime(CLOCK_REALTIME, &t2);

	long long elapsed = timespec_ms(&t2) - timespec_ms(&t1);

	g_terminate = 1;
	pthread_mutex_lock(&g_queue_mutex);
	pthread_cond_broadcast(&g_queue_cond);
	pthread_mutex_unlock(&g_queue_mutex);

	for (int i = 0; i < g_T; i++)
		pthread_join(threads[i], NULL);

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
	fprintf(ft, "%lld\n", elapsed);
	fclose(ft);

	free(g_arr);
	free(g_tmp);
	free(threads);

	return 0;
}
