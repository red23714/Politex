#include "kernel.h"

void command_machine(unsigned char* str)
{
	unsigned char* command = str;
	unsigned char* arg = 0;

	for (unsigned char* p = str; *p != 0; p++)
	{
		if (*p == ' ')
		{
			*p = 0;		 // Заменяем первый пробел на нуль-терминатор
			arg = p + 1; // Второй кусок начинается после пробела
			break;
		}
	}

	if (uc_strcmp(command, "clear") == 0)
	{
		clear_screen(0x07);
	}
	else if (uc_strcmp(command, "info") == 0)
	{
		print_const_str("Zubenko Mikhail Petrovich");
		print_const_str("FASM and GCC");
		print_const_str(mode);
	}
	else if (uc_strcmp(command, "shutdown") == 0)
	{
		clear_screen(0x07);
		outw(0x604, 0x2000);
	}
}
