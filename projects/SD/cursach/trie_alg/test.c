#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>

#define WIDTH 100
#define HEIGHT 100

typedef struct {
    unsigned char r, g, b;
} RGB;

void split_string_into_three_equal_parts(const char *input_string, char **part1, char **part2, char **part3) {
    size_t total_length = strlen(input_string);
    size_t part_length = total_length / 3;
    size_t remainder = total_length % 3;

    *part1 = malloc(part_length + remainder + 2);
    *part2 = malloc(part_length + 2);
    *part3 = malloc(part_length + 2);

    strncpy(*part1 + 1, input_string, part_length + remainder);
    strncpy(*part2 + 1, input_string + part_length + remainder, part_length);
    strncpy(*part3 + 1, input_string + part_length * 2 + remainder, part_length);

    (*part1)[0] = '$';
    (*part1)[part_length + remainder + 1] = '\0';
    
    (*part2)[0] = '$';
    (*part2)[part_length + 1] = '\0';
    
    (*part3)[0] = '$';
    (*part3)[part_length + 1] = '\0';
}

RGB** create_qr_code_mask(const char *data) {
    QRcode *qr = QRcode_encodeString(data, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!qr) {
        printf("Error creating QR code for: %s\n", data);
        return NULL;
    }

    printf("QR mask created: %dx%d modules\n", qr->width, qr->width);

    // Выделяем память для маски
    RGB **mask = malloc(HEIGHT * sizeof(RGB*));
    for (int i = 0; i < HEIGHT; i++) {
        mask[i] = malloc(WIDTH * sizeof(RGB));
        for (int j = 0; j < WIDTH; j++) {
            mask[i][j] = (RGB){0, 0, 0}; // Черный фон
        }
    }

    // Масштабируем QR код
    int scale = WIDTH / qr->width;
    if (scale < 1) scale = 1;
    
    int qr_size = qr->width * scale;
    int offset_x = (WIDTH - qr_size) / 2;
    int offset_y = (HEIGHT - qr_size) / 2;

    // Создаем маску (белые пиксели где должен быть QR код)
    for (int y = 0; y < qr->width; y++) {
        for (int x = 0; x < qr->width; x++) {
            if (qr->data[y * qr->width + x] & 1) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        int px = offset_x + x * scale + sx;
                        int py = offset_y + y * scale + sy;
                        if (px < WIDTH && py < HEIGHT) {
                            mask[py][px] = (RGB){255, 255, 255}; // Белый для маски
                        }
                    }
                }
            }
        }
    }

    QRcode_free(qr);
    return mask;
}

void apply_color_to_mask(RGB **mask, RGB **output, RGB color) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (mask[y][x].r == 255) { // Если в маске белый пиксель
                output[y][x] = color;   // Применяем цвет
            }
        }
    }
}

void combine_color_channels(RGB **red_channel, RGB **green_channel, RGB **blue_channel, const char *output_path) {
    FILE *fp = fopen(output_path, "wb");
    if (!fp) {
        perror("Cannot open output file");
        return;
    }

    // Записываем заголовок PPM
    fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

    // Комбинируем цветовые каналы
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
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
    printf("Combined image saved to %s\n", output_path);
}

void free_image(RGB **image) {
    for (int i = 0; i < HEIGHT; i++) {
        free(image[i]);
    }
    free(image);
}

char* read_file(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Cannot open input file");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);


    char *buffer = malloc(length + 1);
    if (buffer) {
        size_t read = fread(buffer, 1, length, fp);
        buffer[read] = '\0';
        printf("Read %ld bytes from %s\n", read, filename);
    }

    fclose(fp);
    return buffer;
}

void create_qr_from_data(const char *data, const char *output_path) {
    printf("Creating QR code from data (%ld bytes)...\n", strlen(data));
    
    char *data1, *data2, *data3;
    split_string_into_three_equal_parts(data, &data1, &data2, &data3);

    printf("QR parts: %s, %s, %s\n", data1, data2, data3);

    // Создаем маски для каждого QR кода
    RGB **mask1 = create_qr_code_mask(data1);
    RGB **mask2 = create_qr_code_mask(data2);
    RGB **mask3 = create_qr_code_mask(data3);

    if (!mask1 || !mask2 || !mask3) {
        printf("Error creating QR masks\n");
        free(data1);
        free(data2);
        free(data3);
        return;
    }

    // Создаем цветовые каналы
    RGB **red_channel = malloc(HEIGHT * sizeof(RGB*));
    RGB **green_channel = malloc(HEIGHT * sizeof(RGB*));
    RGB **blue_channel = malloc(HEIGHT * sizeof(RGB*));
    
    for (int i = 0; i < HEIGHT; i++) {
        red_channel[i] = malloc(WIDTH * sizeof(RGB));
        green_channel[i] = malloc(WIDTH * sizeof(RGB));
        blue_channel[i] = malloc(WIDTH * sizeof(RGB));
        
        // Инициализируем черным фоном
        for (int j = 0; j < WIDTH; j++) {
            red_channel[i][j] = (RGB){0, 0, 0};
            green_channel[i][j] = (RGB){0, 0, 0};
            blue_channel[i][j] = (RGB){0, 0, 0};
        }
    }

    // Применяем маски к цветовым каналам
    apply_color_to_mask(mask1, red_channel, (RGB){255, 0, 0});    // Красный канал
    apply_color_to_mask(mask2, green_channel, (RGB){0, 255, 0});  // Зеленый канал
    apply_color_to_mask(mask3, blue_channel, (RGB){0, 0, 255});   // Синий канал

    // Комбинируем каналы в одно изображение
    combine_color_channels(red_channel, green_channel, blue_channel, output_path);

    // Освобождаем память
    free(data1);
    free(data2);
    free(data3);
    
    free_image(mask1);
    free_image(mask2);
    free_image(mask3);
    
    free_image(red_channel);
    free_image(green_channel);
    free_image(blue_channel);

    printf("QR code created: %s\n", output_path);
}

