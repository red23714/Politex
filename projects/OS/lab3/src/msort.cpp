#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <atomic>

#include "queue_win.h"

using namespace std;

static vector<int> arr;
static vector<int> temp;

const int THRESHOLD = 1000;

queue_t q;

// ---------------- TASK ----------------
struct Task
{
	int l, r;
	Task* parent;
	int remaining; // atomic-like usage (safe via queue execution order)
};

// ---------------- MERGE ----------------
void merge(int l, int m, int r)
{
	int i = l, j = m + 1, k = l;

	while (i <= m && j <= r)
	{
		if (arr[i] <= arr[j])
			temp[k++] = arr[i++];
		else
			temp[k++] = arr[j++];
	}

	while (i <= m)
		temp[k++] = arr[i++];
	while (j <= r)
		temp[k++] = arr[j++];

	for (int x = l; x <= r; x++)
		arr[x] = temp[x];
}

// ---------------- TASK FUNCTION ----------------
void mergesort_task(void* arg)
{
	Task* t = (Task*)arg;
	int l = t->l;
	int r = t->r;
	Task* parent = t->parent;

	delete t;

	if (l >= r)
	{
		if (parent)
		{
			if (--parent->remaining == 0)
			{
				queue_push(&q, mergesort_task, parent);
			}
		}
		return;
	}

	if (r - l <= THRESHOLD)
	{
		sort(arr.begin() + l, arr.begin() + r + 1);

		if (parent)
		{
			if (--parent->remaining == 0)
			{
				queue_push(&q, mergesort_task, parent);
			}
		}
		return;
	}

	int m = (l + r) / 2;

	Task* current = new Task{l, r, parent, 2};

	Task* left = new Task{l, m, current, 0};
	Task* right = new Task{m + 1, r, current, 0};

	queue_push(&q, mergesort_task, left);
	queue_push(&q, mergesort_task, right);
}

// ---------------- ROOT WRAPPER ----------------
void root_task(void* arg)
{
	Task* t = (Task*)arg;

	mergesort_task(t);
}

// ---------------- MAIN ----------------
int main()
{
	ifstream fin("input.txt");
	ofstream fout("output.txt");
	ofstream ft("time.txt");

	int threads, n;
	fin >> threads >> n;

	arr.resize(n);
	temp.resize(n);

	for (int i = 0; i < n; i++)
		fin >> arr[i];

	queue_init(&q);

	pthread_t* th = new pthread_t[threads];

	for (int i = 0; i < threads; i++)
	{
		pthread_create(&th[i], nullptr, worker, &q);
	}

	Task* root = new Task{0, n - 1, nullptr, 0};

	auto start = chrono::high_resolution_clock::now();

	queue_push(&q, mergesort_task, root);

	queue_stop_and_wait(&q);

	auto end = chrono::high_resolution_clock::now();

	long long ms =
		chrono::duration_cast<chrono::milliseconds>(end - start).count();

	for (int i = 0; i < threads; i++)
		pthread_join(th[i], nullptr);

	fout << threads << "\n";
	fout << n << "\n";

	for (int i = 0; i < n; i++)
	{
		fout << arr[i];
		if (i + 1 < n)
			fout << " ";
	}

	ft << ms << "\n";

	queue_destroy(&q);

	delete[] th;

	return 0;
}
