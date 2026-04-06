// expr.cpp
// Задание: найти количество уникальных разложений числа N на слагаемые (меньшие
// N) ОС: Windows, синхронизация: Mutex + Event Компилятор: Visual Studio 2010
//
// Алгоритм: классический partition DP
//   dp[i][j] = количество способов представить число i в виде суммы слагаемых,
//              каждое из которых не превосходит j.
//   dp[i][j] = dp[i][j-1] + dp[i-j][j]  (берём или не берём слагаемое j)
//   dp[0][j] = 1 для всех j (пустая сумма)
//   Ответ: dp[N][N-1] - 1  (вычитаем само N как разложение, т.к. N < N не
//   выполняется,
//          точнее: слагаемые строго меньше N, поэтому dp[N][N-1])
//
// Параллелизм: вычисление dp-столбцов (по j от 1 до N-1) распределяется между
// потоками пула. Каждый столбец j зависит от столбца j-1, поэтому столбцы
// вычисляются по одному, но строки внутри столбца распределяются по потокам
// пула. Барьер между столбцами реализован через Mutex + два Event (барьерная
// синхронизация).

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ---------- Глобальные данные ----------

static int g_N;			// Входное число
static int g_T;			// Количество потоков пула
static long long* g_dp; // DP-таблица: g_dp[i*(N+1)+j]

// Описание одной «порции» работы для потока пула
// Каждый поток получает диапазон строк [row_from, row_to) для текущего столбца
// j
struct WorkChunk
{
	int row_from;
	int row_to;
};

static WorkChunk* g_chunks;		 // Массив порций, по одной на поток
static volatile int g_current_j; // Текущий столбец j (только читается потоками)

// Барьерная синхронизация (конец фазы):
//   Основной поток ждёт, пока все рабочие не закончат свою часть столбца j,
//   затем переходит к столбцу j+1.
static volatile LONG g_done_count; // Счётчик завершивших потоков в текущей фазе
static HANDLE g_phase_done; // Event (auto-reset): сигнал основному потоку
							//   что g_done_count достиг g_T

// Запуск новой фазы:
//   Основной поток сигналит всем рабочим, что новый столбец готов к обработке.
static HANDLE g_start_event; // Event (manual-reset): сигнал рабочим начать
static HANDLE g_mutex;		 // Mutex для защиты g_done_count (для надёжности)

static volatile int g_terminate; // Флаг завершения: рабочие выходят из цикла

// ---------- Поточная функция ----------

DWORD WINAPI worker_thread(void* param)
{
	int idx = (int)((char*)param - (char*)0);

	while (1)
	{
		// Ждём разрешения начать новую фазу
		WaitForSingleObject(g_start_event, INFINITE);

		// Проверяем флаг завершения
		if (g_terminate)
			break;

		int j = g_current_j;
		int from = g_chunks[idx].row_from;
		int to = g_chunks[idx].row_to;

		// Вычисляем dp[i][j] для i в [from, to)
		// dp[i][j] = dp[i][j-1] + (i >= j ? dp[i-j][j] : 0)
		for (int i = from; i <= to; i++)
		{
			long long val = g_dp[i * (g_N + 1) + (j - 1)]; // dp[i][j-1]
			if (i >= j)
				val += g_dp[(i - j) * (g_N + 1) + j]; // dp[i-j][j]
			g_dp[i * (g_N + 1) + j] = val;
		}

		// Атомарно увеличиваем счётчик завершивших
		LONG cnt = InterlockedIncrement(&g_done_count);
		if (cnt == g_T)
		{
			// Все потоки завершили фазу — сигналим основному
			SetEvent(g_phase_done);
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
	fclose(fin);

	// --- Выделение DP-таблицы ---
	// dp[(N+1) x N], индексация: dp[i][j] = g_dp[i*(N+1)+j]
	g_dp = (long long*)calloc((g_N + 1) * (g_N + 1), sizeof(long long));
	if (!g_dp)
	{
		fprintf(stderr, "Out of memory\n");
		return 1;
	}

	// Базовый случай: dp[0][j] = 1 для всех j
	for (int j = 0; j <= g_N; j++)
		g_dp[0 * (g_N + 1) + j] = 1;

	// --- Создание объектов синхронизации ---
	g_terminate = 0;
	g_done_count = 0;

	// Manual-reset event: основной поток сбросит его перед каждым «ResetEvent»
	g_start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	// Auto-reset event: срабатывает когда все потоки завершили фазу
	g_phase_done = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Mutex (используется при необходимости для защиты данных)
	g_mutex = CreateMutex(NULL, FALSE, NULL);

	// --- Распределение строк по потокам ---
	g_chunks = (WorkChunk*)malloc(g_T * sizeof(WorkChunk));
	{
		int rows = g_N; // строки 1..N
		int per = rows / g_T;
		int rem = rows % g_T;
		int cur = 1;
		for (int t = 0; t < g_T; t++)
		{
			int cnt = per + (t < rem ? 1 : 0);
			g_chunks[t].row_from = cur;
			g_chunks[t].row_to = cur + cnt - 1;
			cur += cnt;
		}
	}

	// --- Создание пула потоков ---
	HANDLE* threads = (HANDLE*)malloc(g_T * sizeof(HANDLE));
	for (int t = 0; t < g_T; t++)
		threads[t] =
			CreateThread(NULL, 0, worker_thread, (char*)0 + t, 0, NULL);

	// --- Замер времени ---
	LARGE_INTEGER freq, t_start, t_end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t_start);

	// --- Основной цикл: вычисляем столбцы j = 1 .. N-1 ---
	// Слагаемые строго меньше N, поэтому максимальное слагаемое = N-1
	for (int j = 1; j <= g_N - 1; j++)
	{
		g_current_j = j;
		g_done_count = 0;

		// Сбрасываем manual-reset event, затем устанавливаем —
		// все рабочие потоки «проснутся» одновременно
		ResetEvent(g_start_event);
		SetEvent(g_start_event);

		// Ждём, пока все потоки завершат обработку столбца j
		WaitForSingleObject(g_phase_done, INFINITE);
	}

	QueryPerformanceCounter(&t_end);
	long long elapsed_ms = (long long)((t_end.QuadPart - t_start.QuadPart) *
									   1000LL / freq.QuadPart);

	// --- Завершение потоков ---
	g_terminate = 1;
	ResetEvent(g_start_event);
	SetEvent(g_start_event); // Будим потоки чтобы они увидели g_terminate
	WaitForMultipleObjects(g_T, threads, TRUE, INFINITE);

	// --- Результат ---
	// dp[N][N-1]: количество разложений N на слагаемые, каждое из которых <=
	// N-1 (т.е. < N) Включает «пустое» разложение? Нет: dp[0]=1 это пустая
	// сумма, но мы ищем dp[N] dp[N][N-1] уже не включает само N как слагаемое
	// (т.к. j идёт до N-1)
	long long answer = g_dp[g_N * (g_N + 1) + (g_N - 1)];

	// --- Запись output.txt ---
	FILE* fout = fopen("output.txt", "w");
	if (!fout)
	{
		fprintf(stderr, "Cannot open output.txt\n");
		return 1;
	}
	fprintf(fout, "%d\n%d\n%lld\n", g_T, g_N, answer);
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
	free(g_dp);
	free(g_chunks);
	for (int t = 0; t < g_T; t++)
		CloseHandle(threads[t]);
	free(threads);
	CloseHandle(g_start_event);
	CloseHandle(g_phase_done);
	CloseHandle(g_mutex);

	return 0;
}
