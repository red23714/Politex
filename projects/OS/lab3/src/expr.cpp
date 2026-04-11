#include <stdio.h>
#include <stdlib.h>

volatile int variants_count = 0;
volatile int all_variants[10000][100];

void clean_array(int* array, int target, int right_num)
{
	for (int i = 1; i < target - right_num + 1; i++)
	{
		array[i] = 1;
	}

	for (int i = target - right_num + 1; i < target; i++)
	{
		array[i] = 0;
	}

	array[0] = right_num;
}

void print_array(int* tmp, int target)
{
	for (int i = 0; i < target; i++)
	{
		if (tmp[i] != 0)
			printf("%d ", tmp[i]);
	}
	printf("\n");
}

void write_variant(int* buf, int target)
{
	print_array(buf, target);
	variants_count++;
}

void count_variants(int target, int right_num)
{
	int tmp[target + 1];
	int state[target + 1];

	clean_array(tmp, target, right_num);
	write_variant(tmp, target);

	clean_array(state, target, right_num);

	int val_pos = 1;

	while (1)
	{
		state[val_pos]++;

		if (state[val_pos] > state[val_pos - 1])
		{
			state[val_pos] = 1;
			val_pos--;
			if (val_pos == 0)
				break;
			continue;
		}

		int sum = 0;
		for (int i = 0; i <= val_pos; i++)
			sum += state[i];
		int rest = target - sum;

		if (rest < 0)
		{
			state[val_pos] = 1;
			val_pos--;
			if (val_pos == 0)
				break;
			continue;
		}

		for (int i = val_pos + 1; i < target + 1; i++)
			state[i] = (i <= val_pos + rest) ? 1 : 0;

		for (int i = 0; i < target + 1; i++)
			tmp[i] = state[i];

		write_variant(tmp, target);

		if (rest > 0)
			val_pos++;
	}
}

void thread_entry(int target, int ind_task, int right_num)
{
	for (int i = 0; i < ind_task; i++)
	{
		if (right_num == 0 && i == 0)
			continue;
		count_variants(target, right_num + i);
	}
}

int main()
{
	int target = 50;
	int threads = 13;

	int individual_task = target / threads;
	int remainder = target - individual_task * threads;

	for (int i = 0; i < threads; i++)
	{
		thread_entry(target, individual_task, i * individual_task);
	}

	printf("Start of remainder\n");

	for (int i = 0; i < remainder; i++)
	{
		thread_entry(target, 1, target - remainder + i);
	}

	printf("%d\n", variants_count);

	return 0;
}
