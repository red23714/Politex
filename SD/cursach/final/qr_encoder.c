#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>
#include "qr_encoder.h"

typedef unsigned char u8;

// --------------------- Шифрование ---------------------

void crypt_data(char* restrict data, const char* restrict password, size_t len)
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
			v += pass[j];
			if (v >= RANGE)
				v -= RANGE;
			buf[i + j] = v + MIN_CHAR;
		}
	}

	for (; i < len; ++i)
	{
		unsigned char v = buf[i] - MIN_CHAR;
		v += pass[i % pass_len];
		if (v >= RANGE)
			v -= RANGE;
		buf[i] = v + MIN_CHAR;
	}
}

// --------------------- Разделение строки ---------------------

void split_string_into_three_equal_parts(const char* input, size_t len,
										 char** p1, char** p2, char** p3)
{
	size_t l1 = len / 3;
	size_t l2 = len / 3;
	size_t l3 = len - l1 - l2;

	char* a = malloc(l1 + 2);
	char* b = malloc(l2 + 2);
	char* c = malloc(l3 + 2);

	if (!a || !b || !c)
	{
		perror("malloc");
		exit(1);
	}

	snprintf(a, l1 + 2, "$%.*s", (int)l1, input);
	snprintf(b, l2 + 2, "$%.*s", (int)l2, input + l1);
	snprintf(c, l3 + 2, "$%.*s", (int)l3, input + l1 + l2);

	*p1 = a;
	*p2 = b;
	*p3 = c;
}

// --------------------- Создание маски ---------------------

RGB** create_qr_mask(const char* restrict data, ImageSize* restrict size)
{
	QRcode* qr = QRcode_encodeString(data, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	if (!qr)
		return NULL;

	const int modules = qr->width;

	int scale = BASE_SCALE;
	if (modules > 40)
		scale = 2;
	else if (modules > 25)
		scale = 3;

	const int pix = modules * scale;
	const int pad = (int)(pix * PADDING_RATIO);

	size->width = pix + pad * 2;
	size->height = pix + pad * 2;

	RGB** mask = malloc(size->height * sizeof(RGB*));
	RGB* data_buf = calloc(size->height * size->width, sizeof(RGB));

	for (int y = 0; y < size->height; ++y)
		mask[y] = data_buf + y * size->width;

	const u8* restrict q = qr->data;

	for (int y = 0; y < modules; ++y)
		for (int x = 0; x < modules; ++x)
			if (q[y * modules + x] & 1)
			{
				const int base_x = pad + x * scale;
				const int base_y = pad + y * scale;

				for (int sy = 0; sy < scale; ++sy)
				{
					RGB* row = mask[base_y + sy] + base_x;
#pragma GCC unroll 4
					for (int sx = 0; sx < scale; ++sx)
						row[sx] = (RGB){255, 255, 255};
				}
			}

	QRcode_free(qr);
	return mask;
}

RGB** alloc_image(ImageSize s)
{
	RGB** rows = malloc(s.height * sizeof(RGB*));
	RGB* buf = calloc(s.width * s.height, sizeof(RGB));

	for (int y = 0; y < s.height; ++y)
		rows[y] = buf + y * s.width;

	return rows;
}

void free_image(RGB** img)
{
	if (!img)
		return;
	free(img[0]);
	free(img);
}

// --------------------- Центрирование ---------------------

void apply_color_centered(RGB** restrict mask, ImageSize ms, RGB** restrict out,
						  ImageSize os, RGB color)
{
	const int off_x = (os.width - ms.width) >> 1;
	const int off_y = (os.height - ms.height) >> 1;

	for (int y = 0; y < ms.height; ++y)
	{
		RGB* m = mask[y];
		RGB* o = out[y + off_y] + off_x;

		for (int x = 0; x < ms.width; ++x)
		{
			if (m[x].r) // белый пиксель маски
			{
				o[x].r |= color.r;
				o[x].g |= color.g;
				o[x].b |= color.b;
			}
		}
	}
}

// --------------------- PPM в память ---------------------

void ppm_to_memory(RGB** restrict img, ImageSize s,
				   unsigned char** restrict out_buf, size_t* restrict out_size)
{
	char header[64];
	int hlen =
		snprintf(header, sizeof(header), "P6\n%d %d\n255\n", s.width, s.height);

	size_t pixels = (size_t)s.width * s.height * 3;
	*out_size = hlen + pixels;

	unsigned char* buf = malloc(*out_size);
	memcpy(buf, header, hlen);

	unsigned char* p = buf + hlen;

	for (int y = 0; y < s.height; ++y)
	{
		RGB* row = img[y];
#pragma GCC unroll 4
		for (int x = 0; x < s.width; ++x)
		{
			*p++ = row[x].r;
			*p++ = row[x].g;
			*p++ = row[x].b;
		}
	}

	*out_buf = buf;
}

// --------------------- Генерация QR ---------------------

int generate_qr_mem(char* original_string, int size, char* password,
					u8** out_ppm, size_t* out_size)
{
	char *d1, *d2, *d3;
	crypt_data(original_string, password, size);

	split_string_into_three_equal_parts(original_string, size, &d1, &d2, &d3);

	ImageSize s1, s2, s3;
	RGB** m1 = create_qr_mask(d1, &s1);
	RGB** m2 = create_qr_mask(d2, &s2);
	RGB** m3 = create_qr_mask(d3, &s3);

	free(d1);
	free(d2);
	free(d3);

	ImageSize final_size = {.width = MAX(s1.width, MAX(s2.width, s3.width)),
							.height =
								MAX(s1.height, MAX(s2.height, s3.height))};

	RGB** img = alloc_image(final_size);

	apply_color_centered(m1, s1, img, final_size, (RGB){255, 0, 0});
	apply_color_centered(m2, s2, img, final_size, (RGB){0, 255, 0});
	apply_color_centered(m3, s3, img, final_size, (RGB){0, 0, 255});

	ppm_to_memory(img, final_size, out_ppm, out_size);

	free_image(img);
	free_image(m1);
	free_image(m2);
	free_image(m3);

	return 1;
}
