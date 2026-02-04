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
	if (input_size < sizeof(unsigned int) ||
		input_size % sizeof(unsigned int) != 0)
		return 0;

	int n_codes = (int)(input_size / sizeof(unsigned int));
	const unsigned int* codes = (const unsigned int*)input;

	// Выделяем память для словаря
	unsigned char** dict =
		(unsigned char**)malloc(65536 * sizeof(unsigned char*));
	unsigned short* dict_lens =
		(unsigned short*)malloc(65536 * sizeof(unsigned short));

	if (!dict || !dict_lens)
	{
		free(dict);
		free(dict_lens);
		return 0;
	}

	// Инициализация словаря с одиночными байтами
	for (int i = 0; i < 256; i++)
	{
		dict[i] = (unsigned char*)malloc(1);
		if (!dict[i])
		{
			// Освобождаем уже выделенное
			for (int j = 0; j < i; j++)
				free(dict[j]);
			free(dict);
			free(dict_lens);
			return 0;
		}
		dict[i][0] = (unsigned char)i;
		dict_lens[i] = 1;
	}

	int next_code = 256;

	// Вместо предварительного выделения большого буфера, будем наращивать по
	// мере необходимости
	size_t out_alloc = 1024; // Начинаем с небольшого размера
	unsigned char* out_buf = (unsigned char*)malloc(out_alloc);
	if (!out_buf)
	{
		for (int i = 0; i < 256; i++)
			free(dict[i]);
		free(dict);
		free(dict_lens);
		return 0;
	}
	size_t out_pos = 0;

	// Обрабатываем первый код
	if (n_codes == 0)
	{
		free(out_buf);
		for (int i = 0; i < 256; i++)
			free(dict[i]);
		free(dict);
		free(dict_lens);
		return 0;
	}

	unsigned int old_code = codes[0];
	if (old_code >= 256)
	{
		free(out_buf);
		for (int i = 0; i < 256; i++)
			free(dict[i]);
		free(dict);
		free(dict_lens);
		return 0;
	}

	size_t old_len = dict_lens[old_code];

	// Проверяем и расширяем буфер если нужно
	if (out_pos + old_len > out_alloc)
	{
		size_t new_alloc = out_alloc;
		while (new_alloc < out_pos + old_len)
			new_alloc *= 2;
		unsigned char* tmp = (unsigned char*)realloc(out_buf, new_alloc);
		if (!tmp)
		{
			free(out_buf);
			for (int i = 0; i < 256; i++)
				free(dict[i]);
			free(dict);
			free(dict_lens);
			return 0;
		}
		out_buf = tmp;
		out_alloc = new_alloc;
	}

	memcpy(out_buf + out_pos, dict[old_code], old_len);
	out_pos += old_len;

	// Обрабатываем остальные коды
	for (int i = 1; i < n_codes; i++)
	{
		unsigned int new_code = codes[i];
		unsigned char* s = NULL;
		size_t slen = 0;

		if (new_code < (unsigned int)next_code)
		{
			s = dict[new_code];
			slen = dict_lens[new_code];
		}
		else if (new_code == (unsigned int)next_code)
		{
			// KWC special case
			slen = dict_lens[old_code] + 1;
			unsigned char* tmp_buf = (unsigned char*)malloc(slen);
			if (!tmp_buf)
			{
				// Пропускаем этот код и продолжаем
				continue;
			}
			memcpy(tmp_buf, dict[old_code], dict_lens[old_code]);
			tmp_buf[slen - 1] = dict[old_code][0];
			s = tmp_buf;
		}
		else
		{
			// Некорректный код - пропускаем
			continue;
		}

		// Расширяем буфер при необходимости
		if (out_pos + slen > out_alloc)
		{
			size_t new_alloc = out_alloc;
			while (new_alloc < out_pos + slen)
				new_alloc *= 2;
			unsigned char* tmp = (unsigned char*)realloc(out_buf, new_alloc);
			if (!tmp)
			{
				if (s != dict[new_code])
					free(s); // Освобождаем временный буфер если был выделен
				break;
			}
			out_buf = tmp;
			out_alloc = new_alloc;
		}

		memcpy(out_buf + out_pos, s, slen);
		out_pos += slen;

		// Добавляем новое слово в словарь
		if (next_code < 65536)
		{
			size_t new_len = dict_lens[old_code] + 1;
			dict[next_code] = (unsigned char*)malloc(new_len);
			if (dict[next_code])
			{
				memcpy(dict[next_code], dict[old_code], dict_lens[old_code]);
				dict[next_code][dict_lens[old_code]] = s[0];
				dict_lens[next_code] = (unsigned short)new_len;
			}
			next_code++;
		}

		// Освобождаем временный буфер если он был выделен
		if (s != dict[new_code] && s != dict[old_code])
			free(s);

		old_code = new_code;
	}

	// Копируем результат в выходной буфер
	*output = (unsigned char*)malloc(out_pos);
	if (*output && out_pos > 0)
	{
		memcpy(*output, out_buf, out_pos);
	}

	// Освобождаем ресурсы
	free(out_buf);
	for (int i = 0; i < next_code; i++)
	{
		free(dict[i]);
	}
	free(dict);
	free(dict_lens);

	return out_pos;
}
