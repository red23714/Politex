// Эта функция обязательно должна быть первой, т.к. этот код компилируется в
// бинарный, и загрузчик передает управление по адресу первой инструкции
// бинарного образа ядра ОС
__asm("jmp kmain");

#define VIDEO_BUF_PTR (0xb8000)

#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)

#define PIC1_PORT (0x20)
#define CURSOR_PORT (0x3D4)

#define VIDEO_WIDTH (80)

// Селектор секции кода, установленный загрузчиком ОС
#define GDT_CS (0x8)

typedef enum
{
	KEY_NONE = 0,
	KEY_ESC = 1,
	KEY_1 = 2,
	KEY_2 = 3,
	KEY_3 = 4,
	KEY_4 = 5,
	KEY_5 = 6,
	KEY_6 = 7,
	KEY_7 = 8,
	KEY_8 = 9,
	KEY_9 = 10,
	KEY_0 = 11,
	KEY_MINUS = 12,
	KEY_EQUAL = 13,
	KEY_BACKSPACE = 14,
	KEY_TAB = 15,
	KEY_Q = 16,
	KEY_W = 17,
	KEY_E = 18,
	KEY_R = 19,
	KEY_T = 20,
	KEY_Y = 21,
	KEY_U = 22,
	KEY_I = 23,
	KEY_O = 24,
	KEY_P = 25,
	KEY_LBRACKET = 26,
	KEY_RBRACKET = 27,
	KEY_ENTER = 28,
	KEY_LCTRL = 29,
	KEY_A = 30,
	KEY_S = 31,
	KEY_D = 32,
	KEY_F = 33,
	KEY_G = 34,
	KEY_H = 35,
	KEY_J = 36,
	KEY_K = 37,
	KEY_L = 38,
	KEY_SEMICOLON = 39,
	KEY_APOSTROPHE = 40,
	KEY_GRAVE = 41,
	KEY_LSHIFT = 42,
	KEY_BACKSLASH = 43,
	KEY_Z = 44,
	KEY_X = 45,
	KEY_C = 46,
	KEY_V = 47,
	KEY_B = 48,
	KEY_N = 49,
	KEY_M = 50,
	KEY_COMMA = 51,
	KEY_DOT = 52,
	KEY_SLASH = 53,
	KEY_RSHIFT = 54,
	KEY_KP_MULT = 55, // клавиатура numpad
	KEY_LALT = 56,
	KEY_SPACE = 57,
	KEY_CAPSLOCK = 58,
	KEY_F1 = 59,
	KEY_F2 = 60,
	KEY_F3 = 61,
	KEY_F4 = 62,
	KEY_F5 = 63,
	KEY_F6 = 64,
	KEY_F7 = 65,
	KEY_F8 = 66,
	KEY_F9 = 67,
	KEY_F10 = 68,
	KEY_NUMLOCK = 69,
	KEY_SCROLLLOCK = 70,
	KEY_KP7 = 71,
	KEY_KP8 = 72,
	KEY_KP9 = 73,
	KEY_KP_MINUS = 74,
	KEY_KP4 = 75,
	KEY_KP5 = 76,
	KEY_KP6 = 77,
	KEY_KP_PLUS = 78,
	KEY_KP1 = 79,
	KEY_KP2 = 80,
	KEY_KP3 = 81,
	KEY_KP0 = 82,
	KEY_KP_DOT = 83,
	KEY_F11 = 87,
	KEY_F12 = 88,
	KEY_HOME = 71, // пример, можно уточнить точные сканкоды стрелок и навигации
	KEY_UP = 72,
	KEY_PAGEUP = 73,
	KEY_LEFT = 75,
	KEY_RIGHT = 77,
	KEY_END = 79,
	KEY_DOWN = 80,
	KEY_PAGEDOWN = 81,
	KEY_INSERT = 82,
	KEY_DELETE = 83
	// остальное можно добавить по необходимости
} Keycode;

unsigned int curs_x = 0, curs_y = 0;

static inline unsigned char inb(unsigned short port)
{
	unsigned char data;
	asm volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
	return data;
}

static inline void outb(unsigned short port, unsigned char data)
{
	asm volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

// Структура описывает данные об обработчике прерывания
struct idt_entry
{
	unsigned short base_lo;	 // Младшие биты адреса  обработчика
	unsigned short segm_sel; // Селектор сегмента кода
	unsigned char always0;	 // Этот байт всегда 0
	unsigned char
		flags; // Флаги тип. Флаги: P, DPL, Типы - это константы - IDT_TYPE...
	unsigned short base_hi; // Старшие биты адреса обработчика
} __attribute__((packed));	// Выравнивание запрещено

// Структура, адрес который передается как аргумент команды lidt
struct idt_ptr
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp;		 // Описатель таблицы для команды lidt

// Пустой обработчик прерываний.
void default_intr_handler()
{
	asm("pusha");
	// TODO обработчик прерываний
	asm("popa; leave; iret");
}

typedef void (*intr_handler)();
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

void keyboard_machine(int scan_code, bool is_pressed);

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

	int len = 0;
	while (*str)
	{
		video_buf[0] = (unsigned char)*str; // Символ(код)
		video_buf[1] = color;				// Цвет символа и фона

		video_buf += 2;
		str++;

		len++;
	}
}

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

void print_str(unsigned char* ptr)
{
	out_str(0x07, ptr, curs_y, 0);

	cursor_moveto(curs_y + 1, 0);
}

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

void command_machine(unsigned char* str);

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
		print_const_str("bm");
	}
}

extern "C" int kmain()
{
	const char* hello = "Welcum to GoydaOS";

	clear_screen(0x07);
	print_const_str(hello);

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
