#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zbar.h>

#define MIN_CHAR 32	 // '!'
#define MAX_CHAR 126 // '~'
#define RANGE (MAX_CHAR - MIN_CHAR + 1)

typedef struct
{
	unsigned char r, g, b;
} RGB;

void decrypt_data(char* restrict data, const char* restrict password,
				  size_t len)
{
	const size_t pass_len = strlen(password);
	const unsigned char* restrict pass = (const unsigned char*)password;
	unsigned char* restrict buf = (unsigned char*)data;

	size_t i = 0;

	for (; i + pass_len <= len; i += pass_len)
	{
#pragma GCC unroll 8
		for (size_t j = 0; j < pass_len; ++j)
		{
			unsigned char v = buf[i + j] - MIN_CHAR;
			// обратное действие
			if (v < pass[j])
				v += RANGE;
			v -= pass[j];
			buf[i + j] = v + MIN_CHAR;
		}
	}

	for (; i < len; ++i)
	{
		unsigned char v = buf[i] - MIN_CHAR;
		if (v < pass[i % pass_len])
			v += RANGE;
		v -= pass[i % pass_len];
		buf[i] = v + MIN_CHAR;
	}
}

// Функция для чтения PPM файла
RGB* read_ppm(const char* filename, int* width, int* height)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		return NULL;

	char format[3];
	int maxval;
	if (fscanf(fp, "%2s", format) != 1)
	{
		fclose(fp);
		return NULL;
	}
	if (fscanf(fp, "%d %d %d", width, height, &maxval) != 3)
	{
		fclose(fp);
		return NULL;
	}
	fgetc(fp);
	if (strcmp(format, "P6") != 0)
	{
		fclose(fp);
		return NULL;
	}

	size_t bufsize = (size_t)(*width) * (*height);
	RGB* buf = malloc(sizeof(RGB) * bufsize);
	if (!buf)
	{
		fclose(fp);
		return NULL;
	}

	fread(buf, sizeof(RGB), bufsize, fp);
	fclose(fp);
	return buf;
}

// Функция для извлечения канала с учетом цветного наложения
static inline void extract_color_channel(const RGB* restrict src,
										 unsigned char* restrict dst, int width,
										 int height, char channel)
{
	const int n = width * height;
	const unsigned char offset = 0; // для выбора канала
	unsigned char r_shift = 0, g_shift = 1, b_shift = 2;

	switch (channel)
	{
	case 'r':
		for (int i = 0; i < n; i++)
			dst[i] = 255 - src[i].r;
		break;
	case 'g':
		for (int i = 0; i < n; i++)
			dst[i] = 255 - src[i].g;
		break;
	case 'b':
		for (int i = 0; i < n; i++)
			dst[i] = 255 - src[i].b;
		break;
	default:
		memset(dst, 0, n);
	}
}

// Кастомная функция очистки для zbar
static void zbar_cleanup_handler(zbar_image_t* image)
{
	const void* data = zbar_image_get_data(image);
	if (data)
	{
		free((void*)data);
	}
}

char* decode_qr_with_zbar(unsigned char* gray_image, int width, int height)
{
	zbar_image_scanner_t* scanner = zbar_image_scanner_create();
	zbar_image_t* image = zbar_image_create();

	if (!scanner || !image)
	{
		fprintf(stderr, "Failed to create zbar objects\n");
		if (scanner)
			zbar_image_scanner_destroy(scanner);
		if (image)
			zbar_image_destroy(image);
		return NULL;
	}

	// Настраиваем сканер
	zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);

	// Создаем копию данных для zbar
	unsigned char* image_data = malloc(width * height);
	memcpy(image_data, gray_image, width * height);

	// Создаем изображение для zbar
	zbar_image_set_format(image, zbar_fourcc('Y', '8', '0', '0'));
	zbar_image_set_size(image, width, height);
	zbar_image_set_data(image, image_data, width * height,
						zbar_cleanup_handler);

	// Сканируем изображение
	int n = zbar_scan_image(scanner, image);

	char* result = NULL;

	if (n > 0)
	{
		// Получаем первый символ (результат)
		const zbar_symbol_t* symbol = zbar_image_first_symbol(image);
		for (; symbol; symbol = zbar_symbol_next(symbol))
		{
			zbar_symbol_type_t type = zbar_symbol_get_type(symbol);
			if (type == ZBAR_QRCODE)
			{
				const char* data = zbar_symbol_get_data(symbol);
				if (data)
				{
					result = strdup(data);
					printf("Decoded QR: %s\n", result);
					break;
				}
			}
		}
	}
	else
	{
		printf("No QR codes found in this channel\n");
	}

	zbar_image_destroy(image);
	zbar_image_scanner_destroy(scanner);

	return result;
}

// Функция для объединения трех частей
char* combine_parts(const char* part1, const char* part2, const char* part3)
{
	if (!part1 || !part2 || !part3)
	{
		return NULL;
	}

	// Пропускаем первый символ '$' если он есть
	const char* p1 = (part1[0] == '$') ? part1 + 1 : part1;
	const char* p2 = (part2[0] == '$') ? part2 + 1 : part2;
	const char* p3 = (part3[0] == '$') ? part3 + 1 : part3;

	size_t len1 = strlen(p1);
	size_t len2 = strlen(p2);
	size_t len3 = strlen(p3);

	char* result = malloc(len1 + len2 + len3 + 1);
	if (!result)
	{
		return NULL;
	}

	strcpy(result, p1);
	strcat(result, p2);
	strcat(result, p3);

	return result;
}

// Функция для освобождения памяти изображения
void free_image(RGB** image, int height)
{
	if (!image)
		return;

	for (int i = 0; i < height; i++)
	{
		free(image[i]);
	}
	free(image);
}

// Главная функция декодирования
char* decode_qr_image(const char* filename, const char* password)
{
	int width, height;
	RGB* image = read_ppm(filename, &width, &height);
	if (!image)
		return NULL;

	size_t n = (size_t)width * height;
	unsigned char *red = malloc(n), *green = malloc(n), *blue = malloc(n);
	if (!red || !green || !blue)
		goto cleanup;

	extract_color_channel(image, red, width, height, 'r');
	extract_color_channel(image, green, width, height, 'g');
	extract_color_channel(image, blue, width, height, 'b');

	char* r_text = decode_qr_with_zbar(red, width, height);
	char* g_text = decode_qr_with_zbar(green, width, height);
	char* b_text = decode_qr_with_zbar(blue, width, height);

	free(red);
	free(green);
	free(blue);
	free(image);

	if (!r_text || !g_text || !b_text)
	{
		free(r_text);
		free(g_text);
		free(b_text);
		return NULL;
	}

	char* combined = combine_parts(r_text, g_text, b_text);
	free(r_text);
	free(g_text);
	free(b_text);

	if (!combined)
		return NULL;

	decrypt_data(combined, password, strlen(combined));
	return combined;

cleanup:
	free(red);
	free(green);
	free(blue);
	free(image);
	return NULL;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s <input_ppm_file> <password>\n", argv[0]);
		printf("Example: %s qr.ppm mypassword\n", argv[0]);
		return 1;
	}

	const char* input_file = argv[1];
	const char* password = argv[2];

	printf("Starting QR decoding process...\n");
	printf("Input file: %s\n", input_file);
	printf("Password: %s\n", password);

	char* result = decode_qr_image(input_file, password);
	if (result)
	{
		printf("\n✅ Successfully decoded!\n");
		printf("Final result: %s\n", result);
		free(result);
		return 0;
	}
	else
	{
		printf("\n❌ Failed to decode QR image\n");
		return 1;
	}
}
