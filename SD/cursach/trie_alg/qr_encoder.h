#ifndef QR_ENCODER_H
#define QR_ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 100
#define HEIGHT 100

typedef struct {
    unsigned char r, g, b;
} RGB;

int generate_xor(const char* str);

// Функция разделения строки на три равные части
void split_string_into_three_equal_parts(const char *input_string, char **part1, char **part2, char **part3);

// Создание маски QR-кода из данных
RGB** create_qr_code_mask(const char *data);

// Применение цвета к маске
void apply_color_to_mask(RGB **mask, RGB **output, RGB color);

// Комбинирование цветовых каналов в изображение PPM
void combine_color_channels(RGB **red_channel, RGB **green_channel, RGB **blue_channel, const char *output_path);

// Освобождение памяти изображения
void free_image(RGB **image);

// Основная функция генерации QR-кода
int generate_qr(char* original_string, char* password);

// Функция извлечения цветовых каналов из изображения
void extract_color_channels(const char *input_path, RGB ***red_channel, RGB ***green_channel, RGB ***blue_channel);

// Функция создания маски из цветового канала
RGB** create_mask_from_channel(RGB **channel, unsigned char threshold);

// Функция декодирования QR-кода из маски
char* decode_qr_from_mask(RGB **mask);

// Основная функция декодирования QR-кода
int decode_qr(const char *input_image_path, const char *password);

#endif // QR_ENCODER_H
