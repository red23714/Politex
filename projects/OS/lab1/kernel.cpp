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
#define GDT_CS (0x08)

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
		intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR, default_intr_handler);
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

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
	unsigned short new_pos = strnum * VIDEO_WIDTH + pos;

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
}

void out_str(int color, const char* ptr, unsigned int strnum)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf +=
		80 * 2 * strnum; // В зависимости от номера строки мы указываем смещение
						 // в видеобуфере где будет отображаться наша строка

	while (*ptr)
	{
		video_buf[0] = (unsigned char)*ptr; // Символ(код)
		video_buf[1] = color;				// Цвет символа и фона

		video_buf += 2;
		ptr++;
	}

	cursor_moveto(strnum + 1, 0);
}

void out_char(int color, const char ch, unsigned int strnum, unsigned int pos)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf += VIDEO_WIDTH * 2 * strnum + pos;

	video_buf[0] = (unsigned char)ch;
	video_buf[1] = color;

	cursor_moveto(strnum, pos + 1);
}

void keyb_process_keys()
{
	if (inb(0x64) & 0x01)
	{
		unsigned char scan_code;
		unsigned char state;

		scan_code = inb(0x60);

		if (scan_code < 128)
		{
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

extern "C" int kmain()
{
	const char* hello = "Welcum to GoydaOS (gcc edition)";

	clear_screen(0x07);
	out_str(0x07, hello, 0);
	out_char(0x07, 'h', 1, 0);

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
