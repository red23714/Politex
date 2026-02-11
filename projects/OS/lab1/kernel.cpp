// Эта функция обязательно должна быть первой, т.к. этот код компилируется в
// бинарный, и загрузчик передает управление по адресу первой инструкции
// бинарного образа ядра ОС
__asm("jmp kmain");

#define VIDEO_BUF_PTR (0xb8000)

void clear_screen(int color)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;

	for (int i = 0; i < 80 * 25; i++)
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
}

const char* g_test = "This is zig heil string!";

extern "C" int kmain()
{
	const char* hello = "Welcum to GoydaOS (gcc edition)";

	clear_screen(0x07);
	out_str(0x07, hello, 0);
	out_str(0x07, g_test, 1);

	while (1)
	{
		asm("hlt");
	}

	return 0;
}
