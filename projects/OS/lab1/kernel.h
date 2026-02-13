#ifndef KERNEL_H
#define KERNEL_H

#define VIDEO_BUF_PTR (0xb8000)
#define IDT_TYPE_INTR (0x0E)
#define PIC1_PORT (0x20)
#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80)
#define GDT_CS (0x8)
#define BOOT_MODE_ADDR (*(int*)0x500)

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

extern bool bm_mode;

extern unsigned int curs_x, curs_y;

extern unsigned char* templ;

// Функции ввода-вывода
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

static inline void outw(unsigned short port, unsigned short data)
{
	asm volatile("outw %w0, %w1" : : "a"(data), "Nd"(port));
}

// Функции работы с экраном
void cursor_moveto(unsigned int strnum, unsigned int pos);
void clear_screen(int color);
void out_str(int color, unsigned char* str, unsigned int strnum,
			 unsigned int pos);

void out_char(int color, unsigned char ch, unsigned int strnum,
			  unsigned int pos);
void print_str(unsigned char* ptr);
void print_const_str(const char* str);

// Функции клавиатуры
void keyboard_machine(int scan_code, bool is_pressed);
void keyb_process_keys();
void keyb_handler();
void keyb_init();

// Функции IDT
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

typedef void (*intr_handler)();
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags,
					  intr_handler hndlr);
void intr_init();
void intr_start();
void intr_enable();
void intr_disable();

// Команды
void command_machine(unsigned char* str);

// Функции работы со строками
void clear_str(unsigned char* str);
int uc_strcmp(const unsigned char* s1, const char* s2);
void to_upcase(unsigned char* in, unsigned char* out);
void to_downcase(unsigned char* in, unsigned char* out);
int uc_strlen(unsigned char* s);
void uc_strcp(unsigned char* s1, unsigned char* s2);
void int_to_str(int value, unsigned char* buffer);
int boyer_moore_search(unsigned char* text);
void print_bm_table_simple(unsigned char* pattern);
int std_search(unsigned char* text);

#endif
