#include <qrencode.h>
#include "qr_encoder.h"

// Генерация псевдослучайной последовательности на основе пароля
void generate_sequence(const char *password, int *seq, size_t len) {
    size_t pass_len = strlen(password);
    unsigned int seed = 0;
    
    // Создаем seed из пароля
    for (size_t i = 0; i < pass_len; i++) {
        seed = seed * 31 + password[i];
    }
    
    srand(seed);
    for (size_t i = 0; i < len; i++) {
        seq[i] = rand() % RANGE;
    }
}

// Шифрование/дешифрование
void crypt_data(char *data, const char *password, size_t len) {
    int *sequence = malloc(len * sizeof(int));
    if (!sequence) return;
    
    generate_sequence(password, sequence, len);
    
    for (size_t i = 0; i < len; i++) {
        // Преобразуем символ в позицию в диапазоне 0-(RANGE-1)
        int pos = data[i] - MIN_CHAR;
        
        // XOR с псевдослучайной последовательностью
        pos = pos ^ sequence[i];
        
        // Обеспечиваем цикличность в пределах диапазона
        pos = pos % RANGE;
        if (pos < 0) pos += RANGE;
        
        // Возвращаем в символьный вид
        data[i] = pos + MIN_CHAR;
    }
    
    free(sequence);
}

void split_string_into_three_equal_parts(const char *input_string, char **part1, char **part2, char **part3) {
    size_t total_length = strlen(input_string);
    size_t part_length = total_length / 3;
    size_t remainder = total_length % 3;

    *part1 = malloc(part_length + remainder + 2);
    *part2 = malloc(part_length + 2);
    *part3 = malloc(part_length + 2);

    // Сначала устанавливаем '$'
    (*part1)[0] = '$';
    (*part2)[0] = '$';
    (*part3)[0] = '$';

    // Затем копируем данные
    memcpy(*part1 + 1, input_string, part_length + remainder);
    memcpy(*part2 + 1, input_string + part_length + remainder, part_length);
    memcpy(*part3 + 1, input_string + part_length * 2 + remainder, part_length);

    // И только потом устанавливаем нуль-терминаторы
    (*part1)[part_length + remainder + 1] = '\0';
    (*part2)[part_length + 1] = '\0';
    (*part3)[part_length + 1] = '\0';
}

RGB** create_qr_code_mask(const char *data, ImageSize *size) {
    QRcode *qr = QRcode_encodeString(data, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!qr) {
        printf("Error creating QR code for: %s\n", data);
        return NULL;
    }

    // Определяем размер на основе сложности данных
    int qr_module_size = qr->width;
    
    // Автоматически подбираем масштаб в зависимости от размера QR
    int scale = BASE_SCALE;
    if (qr_module_size > 25) scale = 3;
    if (qr_module_size > 40) scale = 2;
    
    int qr_pixel_size = qr_module_size * scale;
    
    // Отступы пропорциональны размеру QR
    int padding = (int)(qr_pixel_size * PADDING_RATIO);
    size->width = qr_pixel_size + padding * 2;
    size->height = qr_pixel_size + padding * 2;

    printf("QR %dx%d modules -> %dx%d pixels (scale: %d)\n", 
           qr_module_size, qr_module_size, size->width, size->height, scale);

    // Выделяем память для маски
    RGB **mask = malloc(size->height * sizeof(RGB*));
    for (int i = 0; i < size->height; i++) {
        mask[i] = calloc(size->width, sizeof(RGB)); // Используем calloc для нулевой инициализации
    }

    // Позиционируем QR код по центру
    int offset_x = padding;
    int offset_y = padding;

    // Создаем маску
    for (int y = 0; y < qr_module_size; y++) {
        for (int x = 0; x < qr_module_size; x++) {
            if (qr->data[y * qr_module_size + x] & 1) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        int px = offset_x + x * scale + sx;
                        int py = offset_y + y * scale + sy;
                        if (px < size->width && py < size->height) {
                            mask[py][px] = (RGB){255, 255, 255};
                        }
                    }
                }
            }
        }
    }

    QRcode_free(qr);
    return mask;
}

void apply_color_to_mask(RGB **mask, RGB **output, RGB color, ImageSize size) {
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            if (mask[y][x].r == 255) { // Если в маске белый пиксель
                output[y][x] = color;   // Применяем цвет
            }
        }
    }
}

void combine_color_channels(RGB **red_channel, RGB **green_channel, RGB **blue_channel, 
                           const char *output_path, ImageSize size) {
    FILE *fp = fopen(output_path, "wb");
    if (!fp) {
        perror("Cannot open output file");
        return;
    }

    // Записываем заголовок PPM
    fprintf(fp, "P6\n%d %d\n255\n", size.width, size.height);

    // Комбинируем цветовые каналы
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            RGB pixel;
            pixel.r = red_channel[y][x].r;
            pixel.g = green_channel[y][x].g;
            pixel.b = blue_channel[y][x].b;
            
            fputc(pixel.r, fp);
            fputc(pixel.g, fp);
            fputc(pixel.b, fp);
        }
    }

    fclose(fp);
    printf("Combined image saved to %s (%dx%d)\n", output_path, size.width, size.height);
}

void free_image(RGB **image, ImageSize size) {
    for (int i = 0; i < size.height; i++) {
        free(image[i]);
    }
    free(image);
}

int generate_qr(char* original_string, char* password){ 
    char *data1, *data2, *data3;

    crypt_data(original_string, password, strlen(original_string));

    split_string_into_three_equal_parts(original_string, &data1, &data2, &data3);

    printf("Part 1: %s\n", data1);
    printf("Part 2: %s\n", data2);
    printf("Part 3: %s\n", data3);

    // Создаем маски для каждого QR кода
    ImageSize size1, size2, size3;
    RGB **mask1 = create_qr_code_mask(data1, &size1);
    RGB **mask2 = create_qr_code_mask(data2, &size2);
    RGB **mask3 = create_qr_code_mask(data3, &size3);

    if (!mask1 || !mask2 || !mask3) {
        printf("Error creating masks\n");
        return 0;
    }

    // Определяем максимальный размер для всех каналов
    ImageSize final_size;
    final_size.width = MAX(size1.width, MAX(size2.width, size3.width));
    final_size.height = MAX(size1.height, MAX(size2.height, size3.height));

    // Создаем цветовые каналы
    RGB **red_channel = malloc(final_size.height * sizeof(RGB*));
    RGB **green_channel = malloc(final_size.height * sizeof(RGB*));
    RGB **blue_channel = malloc(final_size.height * sizeof(RGB*));
    
    for (int i = 0; i < final_size.height; i++) {
        red_channel[i] = malloc(final_size.width * sizeof(RGB));
        green_channel[i] = malloc(final_size.width * sizeof(RGB));
        blue_channel[i] = malloc(final_size.width * sizeof(RGB));
        
        // Инициализируем черным фоном
        for (int j = 0; j < final_size.width; j++) {
            red_channel[i][j] = (RGB){0, 0, 0};
            green_channel[i][j] = (RGB){0, 0, 0};
            blue_channel[i][j] = (RGB){0, 0, 0};
        }
    }

    // Применяем маски к цветовым каналам
    apply_color_to_mask(mask1, red_channel, (RGB){255, 0, 0}, size1);
    apply_color_to_mask(mask2, green_channel, (RGB){0, 255, 0}, size2);
    apply_color_to_mask(mask3, blue_channel, (RGB){0, 0, 255}, size3);

    // Комбинируем каналы в одно изображение
    combine_color_channels(red_channel, green_channel, blue_channel, "qr.ppm", final_size);

    // Освобождаем память
    free(data1);
    free(data2);
    free(data3);
    
    free_image(mask1, size1);
    free_image(mask2, size2);
    free_image(mask3, size3);
    
    free_image(red_channel, final_size);
    free_image(green_channel, final_size);
    free_image(blue_channel, final_size);

    return 1;
}
