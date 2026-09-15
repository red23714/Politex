// 52 3087 228 1953125 69 8505 4000 807831573486328125
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ll unsigned long long

void generate_random_string(char* str, int length)
{
	const char charset[] =
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int charset_size = sizeof(charset) - 1;

	for (int i = 0; i < length; i++)
	{
		int key = rand() % charset_size;
		str[i] = charset[key];
	}
}

char* read_file_to_string(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		perror("Не удалось открыть файл");
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL)
	{
		perror("Ошибка выделения памяти");
		fclose(file);
		return NULL;
	}

	size_t bytes_read = fread(buffer, 1, file_size, file);
	buffer[bytes_read] = '\0';

	fclose(file);
	return buffer;
}

typedef struct
{
	ll* data;
	int size;
} dynarr;

void init_buffer(dynarr* buf)
{
	buf->size = 1;
	buf->data = malloc(sizeof(ll));
	buf->data[0] = 1;
}

void add_element(dynarr* buf, ll value)
{
	buf->data = realloc(buf->data, (buf->size + 1) * sizeof(ll));
	buf->data[buf->size++] = value;
}

void compact_buffer(dynarr* buf, int new_start)
{
	int new_size = buf->size - new_start;
	for (int i = 0; i < new_size; i++)
	{
		buf->data[i] = buf->data[new_start + i];
	}

	buf->size = new_size;
	buf->data = realloc(buf->data, buf->size * sizeof(ll));
}

ll give_n_sequence(int n)
{
	dynarr buf;
	init_buffer(&buf);

	int i3 = 0, i5 = 0, i7 = 0;
	int last_compacted = 0;
	int prev_min_used = 0;

	for (int i = 1; i <= n; i++)
	{
		ll next3 = buf.data[i3] * 3;
		ll next5 = buf.data[i5] * 5;
		ll next7 = buf.data[i7] * 7;

		ll next = next3;
		if (next5 < next)
			next = next5;
		if (next7 < next)
			next = next7;

		add_element(&buf, next);

		if (next == next3)
			i3++;
		if (next == next5)
			i5++;
		if (next == next7)
			i7++;

		int min_unused = (i3 < i5) ? i3 : i5;
		min_unused = (min_unused < i7) ? min_unused : i7;

		if (min_unused != prev_min_used)
		{
			compact_buffer(&buf, min_unused);
			i3 -= min_unused;
			i5 -= min_unused;
			i7 -= min_unused;
			last_compacted += min_unused;
		}

		prev_min_used = min_unused;
	}

	ll result = buf.data[n - last_compacted];
	free(buf.data);
	return result;
}

int main()
{
	srand(time(NULL));

	char* pass = read_file_to_string("password.txt");

	if (strncmp(pass, "123456789", 9) != 0)
	{
		printf("Wrong pass\n");
		return 0;
	}

	char* serial = (char*)malloc(16);
	serial[0] = 'K';
	serial[1] = 'E';
	serial[2] = 'Y';
	serial[3] = '$';

	generate_random_string(serial + 4, 10);
	serial[14] = '$';
	serial[15] = '\0';

	FILE* f = fopen("serial.txt", "w");
	fputs(serial, f);
	printf("succsess\n");
	fclose(f);
	free(serial);

	int n;
	while (1)
	{
		if (scanf("%d", &n) != 1)
			break;
		printf("%llu\n", give_n_sequence(n));
	}

	return 0;
}
