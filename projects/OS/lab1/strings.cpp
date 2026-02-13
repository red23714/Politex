#include "kernel.h"

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
	{					// Пока обе строки не закончились
		if (*s1 != *s2) // Если символы не равны
			return (int)(*s1) - (int)(*s2);
		s1++;
		s2++;
	}

	// Если строки разной длины, возвращаем разницу последнего символа и нуля
	return (int)(*s1) - (int)(*s2);
}
