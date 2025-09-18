#ifndef QR_ENCODER_H
#define QR_ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BASE_SCALE 4       // Базовый масштаб вместо фиксированного размера
#define PADDING_RATIO 0.2  // Отступы в процентах от размера QR

#define MIN_CHAR 33   // '!'
#define MAX_CHAR 126  // '~'
#define RANGE (MAX_CHAR - MIN_CHAR + 1)

typedef struct {
    int width;
    int height;
} ImageSize;

typedef struct {
    unsigned char r, g, b;
} RGB;

// Функция разделения строки на три равные части
void split_string_into_three_equal_parts(const char *input_string, char **part1, char **part2, char **part3);

// Создание маски QR-кода из данных
RGB** create_qr_code_mask(const char *data, ImageSize *size);

// Применение цвета к маске
void apply_color_to_mask(RGB **mask, RGB **output, RGB color, ImageSize size);

// Комбинирование цветовых каналов в изображение PPM
void combine_color_channels(RGB **red_channel, RGB **green_channel, RGB **blue_channel, 
                           const char *output_path, ImageSize size);

// Освобождение памяти изображения
void free_image(RGB **image, ImageSize size);

// Основная функция генерации QR-кода
int generate_qr(char* original_string, char* password);

// Функция извлечения цветовых каналов из изображения
void extract_color_channels(const char *input_path, RGB ***red_channel, RGB ***green_channel, RGB ***blue_channel);

// Функция создания маски из цветового канала
RGB** create_mask_from_channel(RGB **channel, unsigned char threshold);

#endif // QR_ENCODER_H
