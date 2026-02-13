#include "kernel.h"

#define ALPHABET_SIZE 256

unsigned char* templ;

void clear_str(unsigned char* str)
{
	if (str == 0)
		return;

	unsigned char* ptr = str;
	while (*ptr != 0)
	{
		*ptr = 0;
		ptr++;
	}
}

int uc_strcmp(const unsigned char* s1, const char* s2)
{
	while (*s1 && *s2)
	{
		if (*s1 != *s2)
			return (int)(*s1) - (int)(*s2);
		s1++;
		s2++;
	}

	return (int)(*s1) - (int)(*s2);
}

void to_upcase(unsigned char* in, unsigned char* out)
{
	clear_str(out);
	int i = 0;
	while (in[i] && i < VIDEO_WIDTH - 1) // защита от переполнения
	{
		unsigned char c = in[i];
		out[i] = (c >= 'a' && c <= 'z') ? c - 32 : c;
		i++;
	}
	out[i] = 0;
}

void to_downcase(unsigned char* in, unsigned char* out)
{
	clear_str(out);
	int i = 0;
	while (in[i] && i < VIDEO_WIDTH - 1) // защита от переполнения
	{
		unsigned char c = in[i];
		out[i] = (c >= 'A' && c <= 'Z') ? c + 32 : c;
		i++;
	}
	out[i] = 0;
}

int uc_strlen(unsigned char* s)
{
	int len = 0;
	while (s[len] != 0)
		len++;
	return len;
}

void uc_strcp(unsigned char* dst, unsigned char* src)
{
	unsigned char* start = dst;

	while (*src != 0)
	{
		*dst = *src;
		dst++;
		src++;
	}

	*dst = 0; // копируем нуль-терминатор
	dst = start;
}

void int_to_str(int value, unsigned char* buffer)
{
	int i = 0;
	int is_negative = 0;

	// Обработка отрицательных чисел
	if (value < 0)
	{
		is_negative = 1;
		value = -value;
	}

	// Специальный случай для 0
	if (value == 0)
	{
		buffer[i++] = '0';
		buffer[i] = 0;
		return;
	}

	// Записываем цифры в обратном порядке
	while (value > 0)
	{
		int digit = value % 10;
		buffer[i++] = '0' + digit;
		value /= 10;
	}

	if (is_negative)
		buffer[i++] = '-';

	buffer[i] = 0;

	// Разворачиваем строку
	int start = 0;
	int end = i - 1;

	while (start < end)
	{
		unsigned char tmp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = tmp;
		start++;
		end--;
	}
}

int std_search(unsigned char* text)
{
	int n = uc_strlen(text);
	int m = uc_strlen(templ);

	if (m == 0)
		return 0;
	if (m > n)
		return -1;

	// Перебираем все возможные начала
	for (int i = 0; i <= n - m; i++)
	{
		int j;
		// Сравниваем шаблон с частью текста, начиная с i
		for (j = 0; j < m; j++)
		{
			if (templ[j] != text[i + j])
			{
				break; // Не совпало
			}
		}
		if (j == m)
		{
			return i; // Нашли! Все символы совпали
		}
		// Если не нашли, цикл переходит к следующему i (сдвиг на 1)
	}
	return -1; // Не нашли
}

int boyer_moore_search(unsigned char* text)
{
	int n = uc_strlen(text);
	int m = uc_strlen(templ);

	if (m == 0)
		return 0;
	if (m > n)
		return -1;

	int badchar[ALPHABET_SIZE] = {-1};

	for (int i = 0; i < m; i++)
	{
		badchar[(unsigned char)templ[i]] = i;
	}

	int shift = 0;

	while (shift <= n - m)
	{
		int j = m - 1;

		while (j >= 0 && templ[j] == text[shift + j])
			j--;

		if (j < 0)
		{
			return shift;
		}
		else
		{
			int bad_char_index = badchar[(unsigned char)text[shift + j]];

			int shift_by_bad_char = j - bad_char_index;

			if (shift_by_bad_char < 1)
				shift_by_bad_char = 1;

			shift += shift_by_bad_char;
		}
	}

	return -1;
}

// Упрощенная функция для вывода таблицы сдвигов
void print_bm_table_simple(unsigned char* pattern)
{
	int m = uc_strlen(pattern);

	if (m == 0)
	{
		print_const_str("Empty pattern\n");
		return;
	}

	print_const_str("BM info: ");

	// Массив для отслеживания уже выведенных символов
	int printed[ALPHABET_SIZE] = {0};

	// Проходим по шаблону слева направо
	for (int i = 0; i < m; i++)
	{
		unsigned char c = pattern[i];

		// Если символ еще не выводили
		if (!printed[c])
		{
			printed[c] = 1;

			// Вычисляем сдвиг
			int shift;

			// Ищем последнее вхождение этого символа
			int last_occ = i;
			for (int j = i + 1; j < m; j++)
			{
				if (pattern[j] == c)
				{
					last_occ = j;
				}
			}

			// Если символ последний в шаблоне
			if (last_occ == m - 1)
			{
				shift = m;
			}
			else
			{
				shift = m - 1 - last_occ;
				if (shift == 0)
					shift = 1; // Минимальный сдвиг = 1
			}

			// Выводим символ и сдвиг
			unsigned char char_str[2] = {c, 0};
			print_str(char_str);
			print_const_str(":");

			unsigned char shift_str[VIDEO_WIDTH];
			int_to_str(shift, shift_str);
			print_str(shift_str);

			// Добавляем пробел между записями
			print_const_str(" ");
		}
	}

	print_const_str("\n");
}
