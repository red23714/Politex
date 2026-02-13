#include "kernel.h"

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
