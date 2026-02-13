#include "kernel.h"

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
