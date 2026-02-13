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

	if (!arg)
		arg = (unsigned char*)"";

	if (uc_strcmp(command, "clear") == 0)
	{
		clear_screen(0x07);
	}
	else if (uc_strcmp(command, "info") == 0)
	{
		print_const_str("Zubenko Mikhail Petrovich\n");
		print_const_str("FASM and GCC\n");

		if (bm_mode)
			print_const_str("BM mode");
		else
			print_const_str("STD mode");

		print_const_str("\n");
	}
	else if (uc_strcmp(command, "shutdown") == 0)
	{
		clear_screen(0x07);
		outw(0x604, 0x2000);
	}
	else if (uc_strcmp(command, "upcase") == 0)
	{
		unsigned char upcase_str[VIDEO_WIDTH];

		to_upcase(arg, upcase_str);
		print_str(upcase_str);
		print_const_str("\n");
	}
	else if (uc_strcmp(command, "downcase") == 0)
	{
		unsigned char downcase_str[VIDEO_WIDTH];

		to_downcase(arg, downcase_str);
		print_str(downcase_str);
		print_const_str("\n");
	}
	else if (uc_strcmp(command, "titlize") == 0)
	{
		unsigned char titlezed_str[VIDEO_WIDTH];

		clear_str(titlezed_str);

		if (arg[0] >= 'a' && arg[0] <= 'z')
			arg[0] -= 32;

		int i = 0;
		while (arg[i] && i < VIDEO_WIDTH - 1)
		{
			titlezed_str[i] = arg[i];
			if (arg[i] == ' ')
			{
				if (arg[i + 1] >= 'a' && arg[i + 1] <= 'z')
				{
					titlezed_str[i + 1] = arg[i + 1] - 32;
					i++;
				}
			}
			i++;
		}

		print_str(titlezed_str);
		print_const_str("\n");
	}
	else if (uc_strcmp(command, "template") == 0)
	{
		clear_str(templ);
		uc_strcp(templ, arg);
		print_const_str("Template '");
		print_str(templ);
		print_const_str("' loaded.");

		if (bm_mode)
		{
			print_bm_table_simple(templ);
		}
		else
		{
			print_const_str("\n");
		}
	}
	else if (uc_strcmp(command, "search") == 0)
	{
		int pos = -1;

		if (bm_mode)
			pos = boyer_moore_search(arg);
		else
			pos = std_search(arg);

		if (pos == -1)
		{
			print_const_str("Not found '");
			print_str(templ);
			print_const_str("'\n");
		}
		else
		{
			unsigned char pos_str[VIDEO_WIDTH];
			int_to_str(pos, pos_str);

			print_const_str("Found '");
			print_str(templ);
			print_const_str("' at pos: ");
			print_str(pos_str);
			print_const_str("\n");
		}
	}

	clear_str(command);
	clear_str(arg);
}
