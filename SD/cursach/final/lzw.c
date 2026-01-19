#include "lzw.h"
#include "hash.h"

size_t lzw_compress_mem(const unsigned char* input, size_t input_size,
						unsigned char** output)
{
	HashTable* dict = create_table();
	for (int i = 0; i < 256; i++)
	{
		char c = (char)i;
		insert(dict, &c, 1, i);
	}
	size_t max_phrase_len = 65536;
	int next_code = 256;
	char* current = malloc(max_phrase_len);
	size_t phrase_len = 0;

	unsigned int* out_buf = malloc(input_size * 4); // максимальный размер
	size_t out_pos = 0;

	for (size_t i = 0; i < input_size; i++)
	{
		current[phrase_len++] = input[i];
		int val;
		if (!search(dict, current, phrase_len, &val))
		{
			if (phrase_len > 1)
			{
				search(dict, current, phrase_len - 1, &val);
				out_buf[out_pos++] = val;
			}
			if (next_code < 65536)
				insert(dict, current, phrase_len, next_code++);
			current[0] = input[i];
			phrase_len = 1;
		}
	}
	if (phrase_len > 0)
	{
		int val;
		search(dict, current, phrase_len, &val);
		out_buf[out_pos++] = val;
	}
	free_table(dict);

	*output = malloc(out_pos * 4);
	memcpy(*output, out_buf, out_pos * 4);
	free(out_buf);
	free(current);
	return out_pos * 4;
}

size_t lzw_decompress_mem(const unsigned char* input, size_t input_size,
						  unsigned char** output)
{
	int n_codes = (int)(input_size / 4);
	const unsigned int* codes = (const unsigned int*)input;

	char* dict[65536] = {0};
	size_t dict_len[65536] = {0};

	// Инициализация словаря с одиночными байтами
	for (int i = 0; i < 256; i++)
	{
		dict[i] = malloc(1);
		dict[i][0] = (char)i;
		dict_len[i] = 1;
	}
	int next_code = 256;

	size_t out_alloc = 1024 * 1024;
	unsigned char* out_buf = malloc(out_alloc);
	size_t out_pos = 0;

	int old_code = codes[0];
	memcpy(out_buf + out_pos, dict[old_code], dict_len[old_code]);
	out_pos += dict_len[old_code];

	for (int i = 1; i < n_codes; i++)
	{
		int new_code = codes[i];
		char* s = NULL;
		size_t slen = 0;

		if (new_code < next_code)
		{
			s = dict[new_code];
			slen = dict_len[new_code];
		}
		else
		{
			// KWC special case
			slen = dict_len[old_code] + 1;
			s = malloc(slen);
			memcpy(s, dict[old_code], dict_len[old_code]);
			s[slen - 1] = dict[old_code][0];
		}

		// Расширяем буфер при необходимости
		if (out_pos + slen > out_alloc)
		{
			while (out_pos + slen > out_alloc)
				out_alloc *= 2;
			out_buf = realloc(out_buf, out_alloc);
		}
		memcpy(out_buf + out_pos, s, slen);
		out_pos += slen;

		// Добавляем новое слово в словарь
		if (next_code < 65536)
		{
			dict[next_code] = malloc(dict_len[old_code] + 1);
			memcpy(dict[next_code], dict[old_code], dict_len[old_code]);
			dict[next_code][dict_len[old_code]] = s[0];
			dict_len[next_code] = dict_len[old_code] + 1;
			next_code++;
		}

		// Временный буфер s нужно освободить только если KWC
		if (new_code >= next_code - 1)
			free(s);

		old_code = new_code;
	}

	// Копируем результат в выходной буфер
	*output = malloc(out_pos);
	memcpy(*output, out_buf, out_pos);
	free(out_buf);

	// Освобождаем все элементы словаря
	for (int i = 0; i < next_code; i++)
		free(dict[i]);

	return out_pos;
}
