#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024

typedef struct {
    unsigned char r, g, b;
} RGB;

void free_image(RGB** image, int height) {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

void skip_comments(FILE *fp) {
    int ch;
    while ((ch = fgetc(fp)) == '#') {
        while ((ch = fgetc(fp)) != '\n' && ch != EOF);
    }
    ungetc(ch, fp);
}

RGB** read_ppm(const char* filename, int* width, int* height) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("Cannot open input file");
        return NULL;
    }

    char format[3];
    int maxval;
    
    // Читаем формат
    if (fscanf(fp, "%2s", format) != 1) {
        fprintf(stderr, "Invalid PPM format\n");
        fclose(fp);
        return NULL;
    }
    
    skip_comments(fp);
    
    // Читаем ширину и высоту
    if (fscanf(fp, "%d %d", width, height) != 2) {
        fprintf(stderr, "Invalid dimensions in PPM\n");
        fclose(fp);
        return NULL;
    }
    
    skip_comments(fp);
    
    // Читаем максимальное значение
    if (fscanf(fp, "%d", &maxval) != 1) {
        fprintf(stderr, "Invalid max value in PPM\n");
        fclose(fp);
        return NULL;
    }
    
    // Пропускаем один байт (обычно это '\n' или пробел)
    fgetc(fp);

    printf("PPM format: %s, dimensions: %dx%d, maxval: %d\n", format, *width, *height, maxval);

    if (strcmp(format, "P6") != 0 && strcmp(format, "P5") != 0 && strcmp(format, "P3") != 0) {
        fprintf(stderr, "Unsupported PPM format: %s\n", format);
        fclose(fp);
        return NULL;
    }

    if (*width > MAX_WIDTH || *height > MAX_HEIGHT) {
        fprintf(stderr, "Image too large: %dx%d (max: %dx%d)\n", *width, *height, MAX_WIDTH, MAX_HEIGHT);
        fclose(fp);
        return NULL;
    }

    // Выделяем память для изображения
    RGB** image = malloc(*height * sizeof(RGB*));
    for (int i = 0; i < *height; i++) {
        image[i] = malloc(*width * sizeof(RGB));
    }

    // Читаем данные изображения в зависимости от формата
    if (strcmp(format, "P6") == 0) {
        // Binary PPM
        for (int y = 0; y < *height; y++) {
            for (int x = 0; x < *width; x++) {
                image[y][x].r = fgetc(fp);
                image[y][x].g = fgetc(fp);
                image[y][x].b = fgetc(fp);
            }
        }
    } else if (strcmp(format, "P5") == 0) {
        // Binary PGM (grayscale) - конвертируем в RGB
        for (int y = 0; y < *height; y++) {
            for (int x = 0; x < *width; x++) {
                unsigned char gray = fgetc(fp);
                image[y][x].r = gray;
                image[y][x].g = gray;
                image[y][x].b = gray;
            }
        }
    } else if (strcmp(format, "P3") == 0) {
        // ASCII PPM
        for (int y = 0; y < *height; y++) {
            for (int x = 0; x < *width; x++) {
                int r, g, b;
                if (fscanf(fp, "%d %d %d", &r, &g, &b) != 3) {
                    fprintf(stderr, "Error reading ASCII PPM data\n");
                    fclose(fp);
                    free_image(image, *height);
                    return NULL;
                }
                image[y][x].r = (unsigned char)r;
                image[y][x].g = (unsigned char)g;
                image[y][x].b = (unsigned char)b;
            }
        }
    }

    fclose(fp);
    printf("Image %s loaded successfully\n", filename);
    return image;
}

void save_channel_as_ppm(RGB** image, int width, int height, const char* filename, char channel) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Cannot open output file");
        return;
    }

    // Записываем заголовок PPM в бинарном формате
    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    // Сохраняем только выбранный канал
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char value;
            
            switch (channel) {
                case 'r': value = image[y][x].r; break;
                case 'g': value = image[y][x].g; break;
                case 'b': value = image[y][x].b; break;
                default: value = 0;
            }
            
            // Сохраняем значение канала во всех трех каналах для монохромного изображения
            fputc(value, fp);
            fputc(value, fp);
            fputc(value, fp);
        }
    }

    fclose(fp);
    printf("Channel %c saved to %s\n", channel, filename);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_ppm_file>\n", argv[0]);
        printf("Example: %s qr.ppm\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    int width, height;
    
    // Читаем исходное изображение
    RGB** image = read_ppm(input_file, &width, &height);
    if (!image) {
        fprintf(stderr, "Failed to read image %s\n", input_file);
        return 1;
    }

    printf("Processing image: %dx%d\n", width, height);

    // Сохраняем отдельные каналы
    save_channel_as_ppm(image, width, height, "red_qr.ppm", 'r');
    save_channel_as_ppm(image, width, height, "green_qr.ppm", 'g');
    save_channel_as_ppm(image, width, height, "blue_qr.ppm", 'b');

    printf("\nQR codes separated successfully:\n");
    printf("- Red channel: red_qr.ppm\n");
    printf("- Green channel: green_qr.ppm\n");
    printf("- Blue channel: blue_qr.ppm\n");
    printf("\nYou can now scan these QR codes separately.\n");

    free_image(image, height);
    return 0;
}
