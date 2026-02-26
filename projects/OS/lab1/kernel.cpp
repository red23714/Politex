#include "kernel.h"

__asm("jmp kmain");

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp;		 // Описатель таблицы для команды lidt

// Пустой обработчик прерываний.
void default_intr_handler()
{
	asm("pusha");
	// TODO обработчик прерываний
	asm("popa; leave; iret");
}

void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags,
					  intr_handler hndlr)
{
	unsigned int hndlr_addr = (unsigned int)hndlr;

	g_idt[num].base_lo = (unsigned short)(hndlr_addr & 0xFFFF);
	g_idt[num].segm_sel = segm_sel;
	g_idt[num].always0 = 0;
	g_idt[num].flags = flags;
	g_idt[num].base_hi = (unsigned short)(hndlr_addr >> 16);
}

void intr_init()
{
	int i;
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

	for (i = 0; i < idt_count; i++)
	{
		if (i != 0x09) // только клавиатура
			intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,
							 default_intr_handler);
	}
}

void intr_start()
{
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

	g_idtp.base = (unsigned int)(&g_idt[0]);
	g_idtp.limit = (sizeof(struct idt_entry) * idt_count) - 1;

	asm("lidt %0" : : "m"(g_idtp));
}

void intr_enable() { asm("sti"); }

void intr_disable() { asm("cli"); }

bool bm_mode;

extern "C" int kmain()
{
	const char* hello = "Welcum to GoydaOS!\n";

	clear_screen(0x07);
	print_const_str(hello);

	int boot_choice = BOOT_MODE_ADDR;

	bool bm = (boot_choice == 0) ? 1 : 0;

	bm_mode = bm;

	intr_disable();
	intr_init();
	keyb_init();
	intr_start();
	intr_enable();

	while (1)
	{
		asm("hlt");
	}

	return 0;
}

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

void print_bm_table_simple(unsigned char* pattern)
{
	int m = uc_strlen(pattern);

	if (m == 0)
	{
		print_const_str("Empty pattern\n");
		return;
	}

	print_const_str("BM info: ");

	int printed[ALPHABET_SIZE] = {0};

	for (int i = 0; i < m; i++)
	{
		unsigned char c = pattern[i];

		if (!printed[c])
		{
			printed[c] = 1;

			int shift;

			int last_occ = i;
			for (int j = i + 1; j < m; j++)
			{
				if (pattern[j] == c)
				{
					last_occ = j;
				}
			}

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

			unsigned char char_str[2] = {c, 0};
			print_str(char_str);
			print_const_str(":");

			unsigned char shift_str[VIDEO_WIDTH];
			int_to_str(shift, shift_str);
			print_str(shift_str);

			print_const_str(" ");
		}
	}

	print_const_str("\n");
}

unsigned int curs_x = 0, curs_y = 0;

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
	unsigned short new_pos = strnum * VIDEO_WIDTH + pos;

	curs_x = pos;
	curs_y = strnum;

	outb(CURSOR_PORT, 0x0F);			   // индекс младшего байта
	outb(CURSOR_PORT + 1, new_pos & 0xFF); // младший байт

	outb(CURSOR_PORT, 0x0E);					  // индекс старшего байта
	outb(CURSOR_PORT + 1, (new_pos >> 8) & 0xFF); // старший байт
}

void clear_screen(int color)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;

	for (int i = 0; i < VIDEO_WIDTH * 25; i++)
	{
		video_buf[0] = ' ';
		video_buf[1] = color;

		video_buf += 2;
	}

	cursor_moveto(0, 0);
}

void out_str(int color, unsigned char* str, unsigned int strnum,
			 unsigned int pos)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf += (80 * strnum + pos) *
				 2; // В зависимости от номера строки мы указываем смещение
					// в видеобуфере где будет отображаться наша строка

	unsigned short len = 0;
	while (*str)
	{
		len++;
		if (*str == '\n')
		{
			cursor_moveto(strnum + 1, 0);
			len = 0;
		}
		else
		{
			video_buf[0] = (unsigned char)*str; // Символ(код)
			video_buf[1] = color;				// Цвет символа и фона

			video_buf += 2;
		}

		str++;
	}

	cursor_moveto(curs_y, curs_x + len);
}

void print_str(unsigned char* ptr) { out_str(0x07, ptr, curs_y, curs_x); }

void print_const_str(const char* str)
{
	unsigned int i = 0;
	unsigned char dest[VIDEO_WIDTH];

	while (str[i] && i < VIDEO_WIDTH - 1) // оставляем место для нуля
	{
		dest[i] = (unsigned char)str[i];
		i++;
	}

	dest[i] = 0;

	print_str(dest);
}

void out_char(int color, unsigned char ch, unsigned int strnum,
			  unsigned int pos)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf += (VIDEO_WIDTH * strnum + pos) * 2;

	video_buf[0] = (unsigned char)ch;
	video_buf[1] = color;
}

unsigned char
	scan_table[128] =
		{
			0,	  27,  '1', '2', '3',  '4',	 '5', '6', '7',	 '8',
			'9',  '0', '-', '=', '\b', '\t', 'q', 'w', 'e',	 'r',
			't',  'y', 'u', 'i', 'o',  'p',	 '[', ']', '\n', 0, // LCtrl=29
			'a',  's', 'd', 'f', 'g',  'h',	 'j', 'k', 'l',	 ';',
			'\'', '`', 0, // LShift=42
			'\\', 'z', 'x', 'c', 'v',  'b',	 'n', 'm', ',',	 '.',
			'/',  0,   0,	0,	 ' ',  0,	 0,
			' ' // пробел на 57
				// Остальные элементы автоматически 0
};

void keyboard_machine(int scan_code, bool is_pressed)
{
	static bool shift_pressed = false;
	static unsigned char command[40] = {0};
	static unsigned char com_len = 0;

	switch (scan_code)
	{
	case KEY_BACKSPACE:
		if (is_pressed)
		{
			if (curs_x > 0)
			{
				com_len--;
				command[com_len] = 0;
				cursor_moveto(curs_y, curs_x - 1);
				out_char(0x07, '\0', curs_y, curs_x);
			}
		}
		break;
	case KEY_LSHIFT:
		shift_pressed = is_pressed;
		break;
	case KEY_ENTER:
		if (is_pressed)
		{
			if (curs_y < 24)
			{
				cursor_moveto(curs_y + 1, 0);
			}
			else
			{
				clear_screen(0x07);
			}

			command_machine(command);
			clear_str(command);
			com_len = 0;
		}
		break;
	default:
		if (is_pressed && com_len < 40)
		{
			unsigned char c = 0;

			if (scan_code < sizeof(scan_table))
				c = scan_table[scan_code];

			if (c != 0 && shift_pressed)
			{
				if (c >= 'a' && c <= 'z')
					c -= 32; // буквы в верхний регистр
				else
				{
					switch (c)
					{
					case '1':
						c = '!';
						break;
					case '2':
						c = '@';
						break;
					case '3':
						c = '#';
						break;
					case '4':
						c = '$';
						break;
					case '5':
						c = '%';
						break;
					case '6':
						c = '^';
						break;
					case '7':
						c = '&';
						break;
					case '8':
						c = '*';
						break;
					case '9':
						c = '(';
						break;
					case '0':
						c = ')';
						break;
					case '-':
						c = '_';
						break;
					case '=':
						c = '+';
						break;
					case '[':
						c = '{';
						break;
					case ']':
						c = '}';
						break;
					case '\\':
						c = '|';
						break;
					case ';':
						c = ':';
						break;
					case '\'':
						c = '"';
						break;
					case ',':
						c = '<';
						break;
					case '.':
						c = '>';
						break;
					case '/':
						c = '?';
						break;
					}
				}
			}

			if (c == 0)
				c = '?'; // для наглядности, если символ пуст

			command[com_len] = c;
			com_len++;

			out_char(0x07, c, curs_y, curs_x);
			cursor_moveto(curs_y, curs_x + 1);
		}
		break;
	}
}

void keyb_process_keys()
{
	if (inb(0x64) & 0x01)
	{
		unsigned char scan_code;
		unsigned char state;

		scan_code = inb(0x60);

		if (scan_code & 0x80)
		{
			scan_code -= 0x80;
			keyboard_machine(scan_code, false);
		}
		else
		{
			keyboard_machine(scan_code, true);
		}
	}
}

void keyb_handler()
{
	asm("pusha");

	keyb_process_keys();

	outb(PIC1_PORT, 0x20);
	asm("popa; leave; iret");
}

void keyb_init()
{
	intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);

	outb(PIC1_PORT + 1, 0xFF ^ 0x02);
}

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
