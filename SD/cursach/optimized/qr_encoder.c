#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>
#include "qr_encoder.h"

typedef unsigned char u8;

// --------------------- Шифрование ---------------------

void crypt_data(char* data, const char* password, size_t len)
{
	size_t pass_len = strlen(password);
	for (size_t i = 0; i < len; i++)
	{
		int pos = data[i] - MIN_CHAR;
		int key = password[i % pass_len] - MIN_CHAR;

		pos = (pos + key) % RANGE;

		data[i] = pos + MIN_CHAR;
	}
}

// --------------------- Разделение строки ---------------------

void split_string_into_three_equal_parts(const char* input, char** p1,
										 char** p2, char** p3)
{
	size_t L = strlen(input);
	size_t part = L / 3;
	size_t rem = L % 3;
	*p1 = malloc(part + rem + 2);
	*p2 = malloc(part + 2);
	*p3 = malloc(part + 2);
	(*p1)[0] = '$';
	(*p2)[0] = '$';
	(*p3)[0] = '$';
	memcpy(*p1 + 1, input, part + rem);
	memcpy(*p2 + 1, input + part + rem, part);
	memcpy(*p3 + 1, input + 2 * part + rem, part);
	(*p1)[part + rem + 1] = 0;
	(*p2)[part + 1] = 0;
	(*p3)[part + 1] = 0;
}

// --------------------- Создание маски ---------------------

RGB** create_qr_mask(const char* data, ImageSize* size)
{
	QRcode* qr = QRcode_encodeString(data, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	if (!qr)
		return NULL;

	int modules = qr->width;
	int scale = BASE_SCALE;
	if (modules > 25)
		scale = 3;
	if (modules > 40)
		scale = 2;

	int pix = modules * scale;
	int pad = (int)(pix * PADDING_RATIO);

	size->width = pix + 2 * pad;
	size->height = pix + 2 * pad;

	RGB** mask = malloc(size->height * sizeof(RGB*));
	for (int y = 0; y < size->height; y++)
		mask[y] = calloc(size->width, sizeof(RGB));

	for (int y = 0; y < modules; y++)
		for (int x = 0; x < modules; x++)
			if (qr->data[y * modules + x] & 1)
				for (int sy = 0; sy < scale; sy++)
					for (int sx = 0; sx < scale; sx++)
					{
						int px = pad + x * scale + sx;
						int py = pad + y * scale + sy;
						if (px < size->width && py < size->height)
							mask[py][px] = (RGB){255, 255, 255};
					}

	QRcode_free(qr);
	return mask;
}

void free_image(RGB** img, ImageSize s)
{
	for (int i = 0; i < s.height; i++)
		free(img[i]);
	free(img);
}

// --------------------- Центрирование ---------------------

void apply_color_centered(RGB** mask, ImageSize mask_size, RGB** out,
						  ImageSize out_size, RGB color)
{
	int offset_x = (out_size.width - mask_size.width) / 2;
	int offset_y = (out_size.height - mask_size.height) / 2;
	for (int y = 0; y < mask_size.height; y++)
		for (int x = 0; x < mask_size.width; x++)
			if (mask[y][x].r == 255)
			{
				int ox = x + offset_x;
				int oy = y + offset_y;
				if (ox >= 0 && ox < out_size.width && oy >= 0 &&
					oy < out_size.height)
					out[oy][ox] = color;
			}
}

// --------------------- PPM в память ---------------------

void ppm_to_memory(RGB** r, RGB** g, RGB** b, ImageSize s, u8** out_buf,
				   size_t* out_size)
{
	char header[64];
	int header_len = sprintf(header, "P6\n%d %d\n255\n", s.width, s.height);
	size_t pixels_len = s.width * s.height * 3;
	*out_size = header_len + pixels_len;
	*out_buf = malloc(*out_size);
	memcpy(*out_buf, header, header_len);
	u8* p = *out_buf + header_len;
	for (int y = 0; y < s.height; y++)
		for (int x = 0; x < s.width; x++)
		{
			*p++ = r[y][x].r;
			*p++ = g[y][x].g;
			*p++ = b[y][x].b;
		}
}

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

// --------------------- Генерация QR в память ---------------------

int generate_qr_mem(char* original_string, char* password, u8** out_ppm,
					size_t* out_size)
{
	crypt_data(original_string, password, strlen(original_string));

	char *d1 = NULL, *d2 = NULL, *d3 = NULL;
	split_string_into_three_equal_parts(original_string, &d1, &d2, &d3);

	ImageSize s1, s2, s3;
	RGB** m1 = create_qr_mask(d1, &s1);
	RGB** m2 = create_qr_mask(d2, &s2);
	RGB** m3 = create_qr_mask(d3, &s3);

	free(d1);
	free(d2);
	free(d3);

	ImageSize final_size;
	final_size.width = MAX(s1.width, MAX(s2.width, s3.width));
	final_size.height = MAX(s1.height, MAX(s2.height, s3.height));

	RGB** R = malloc(final_size.height * sizeof(RGB*));
	RGB** G = malloc(final_size.height * sizeof(RGB*));
	RGB** B = malloc(final_size.height * sizeof(RGB*));
	for (int y = 0; y < final_size.height; y++)
	{
		R[y] = calloc(final_size.width, sizeof(RGB));
		G[y] = calloc(final_size.width, sizeof(RGB));
		B[y] = calloc(final_size.width, sizeof(RGB));
	}

	apply_color_centered(m1, s1, R, final_size, (RGB){255, 0, 0});
	apply_color_centered(m2, s2, G, final_size, (RGB){0, 255, 0});
	apply_color_centered(m3, s3, B, final_size, (RGB){0, 0, 255});

	ppm_to_memory(R, G, B, final_size, out_ppm, out_size);

	free_image(m1, s1);
	free_image(m2, s2);
	free_image(m3, s3);
	free_image(R, final_size);
	free_image(G, final_size);
	free_image(B, final_size);

	return 1;
}
