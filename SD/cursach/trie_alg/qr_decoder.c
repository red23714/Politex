#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zbar.h>

#define MIN_CHAR 32   // '!'
#define MAX_CHAR 126  // '~'
#define RANGE (MAX_CHAR - MIN_CHAR + 1)

typedef struct {
    unsigned char r, g, b;
} RGB;

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

// Функция для чтения PPM файла
RGB** read_ppm(const char* filename, int* width, int* height) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("Cannot open input file");
        return NULL;
    }

    char format[3];
    int maxval;
    
    if (fscanf(fp, "%2s", format) != 1) {
        fprintf(stderr, "Invalid PPM format\n");
        fclose(fp);
        return NULL;
    }
    
    if (fscanf(fp, "%d %d %d", width, height, &maxval) != 3) {
        fprintf(stderr, "Invalid PPM header\n");
        fclose(fp);
        return NULL;
    }
    
    fgetc(fp); // Пропускаем перевод строки

    if (strcmp(format, "P6") != 0) {
        fprintf(stderr, "Unsupported PPM format: %s\n", format);
        fclose(fp);
        return NULL;
    }

    RGB** image = malloc(*height * sizeof(RGB*));
    for (int i = 0; i < *height; i++) {
        image[i] = malloc(*width * sizeof(RGB));
        fread(image[i], sizeof(RGB), *width, fp);
    }

    fclose(fp);
    printf("Loaded image: %s (%dx%d)\n", filename, *width, *height);
    return image;
}

// Функция для извлечения канала с учетом цветного наложения
unsigned char* extract_color_channel(RGB** image, int width, int height, char channel) {
    unsigned char* gray = malloc(width * height * sizeof(unsigned char));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // В вашем кодировщике цветные каналы накладываются на черный фон
            // Нужно извлекать только соответствующий канал
            switch (channel) {
                case 'r': 
                    gray[y * width + x] = image[y][x].r;
                    break;
                case 'g': 
                    gray[y * width + x] = image[y][x].g;
                    break;
                case 'b': 
                    gray[y * width + x] = image[y][x].b;
                    break;
                default: 
                    gray[y * width + x] = 0;
            }
            
            // Инвертируем, так как в QR коде черный = 1, белый = 0
            // Но в вашем кодировщике белый цвет маски становится цветным
            gray[y * width + x] = 255 - gray[y * width + x];
        }
    }
    
    return gray;
}

// Кастомная функция очистки для zbar
static void zbar_cleanup_handler(zbar_image_t *image) {
    const void *data = zbar_image_get_data(image);
    if (data) {
        free((void*)data);
    }
}

// Функция для декодирования QR-кода с помощью zbar
char* decode_qr_with_zbar(unsigned char* gray_image, int width, int height) {
    zbar_image_scanner_t *scanner = zbar_image_scanner_create();
    zbar_image_t *image = zbar_image_create();
    
    if (!scanner || !image) {
        fprintf(stderr, "Failed to create zbar objects\n");
        if (scanner) zbar_image_scanner_destroy(scanner);
        if (image) zbar_image_destroy(image);
        return NULL;
    }
    
    // Настраиваем сканер
    zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
    
    // Создаем копию данных для zbar
    unsigned char* image_data = malloc(width * height);
    memcpy(image_data, gray_image, width * height);
    
    // Создаем изображение для zbar
    zbar_image_set_format(image, zbar_fourcc('Y','8','0','0'));
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, image_data, width * height, zbar_cleanup_handler);
    
    // Сканируем изображение
    int n = zbar_scan_image(scanner, image);
    
    char* result = NULL;
    
    if (n > 0) {
        // Получаем первый символ (результат)
        const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
        for (; symbol; symbol = zbar_symbol_next(symbol)) {
            zbar_symbol_type_t type = zbar_symbol_get_type(symbol);
            if (type == ZBAR_QRCODE) {
                const char *data = zbar_symbol_get_data(symbol);
                if (data) {
                    result = strdup(data);
                    printf("Decoded QR: %s\n", result);
                    break;
                }
            }
        }
    } else {
        printf("No QR codes found in this channel\n");
    }
    
    zbar_image_destroy(image);
    zbar_image_scanner_destroy(scanner);
    
    return result;
}

// Функция для объединения трех частей
char* combine_parts(const char* part1, const char* part2, const char* part3) {
    if (!part1 || !part2 || !part3) {
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
    if (!result) {
        return NULL;
    }
    
    strcpy(result, p1);
    strcat(result, p2);
    strcat(result, p3);
    
    return result;
}

// Функция для освобождения памяти изображения
void free_image(RGB** image, int height) {
    if (!image) return;
    
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

// Главная функция декодирования
char* decode_qr_image(const char* filename, const char* password) {
    int width, height;
    
    // Читаем изображение
    RGB** image = read_ppm(filename, &width, &height);
    if (!image) {
        fprintf(stderr, "Failed to read image: %s\n", filename);
        return NULL;
    }
    
    // Декодируем три канала
    char* red_text = NULL;
    char* green_text = NULL;
    char* blue_text = NULL;
    
    printf("Decoding red channel...\n");
    unsigned char* red_gray = extract_color_channel(image, width, height, 'r');
    red_text = decode_qr_with_zbar(red_gray, width, height);
    free(red_gray);
    
    printf("Decoding green channel...\n");
    unsigned char* green_gray = extract_color_channel(image, width, height, 'g');
    green_text = decode_qr_with_zbar(green_gray, width, height);
    free(green_gray);
    
    printf("Decoding blue channel...\n");
    unsigned char* blue_gray = extract_color_channel(image, width, height, 'b');
    blue_text = decode_qr_with_zbar(blue_gray, width, height);
    free(blue_gray);
    
    // Проверяем, что все три канала декодировались успешно
    if (!red_text || !green_text || !blue_text) {
        fprintf(stderr, "Failed to decode all channels:\n");
        fprintf(stderr, "Red: %s\n", red_text ? red_text : "failed");
        fprintf(stderr, "Green: %s\n", green_text ? green_text : "failed");
        fprintf(stderr, "Blue: %s\n", blue_text ? blue_text : "failed");
        
        free_image(image, height);
        if (red_text) free(red_text);
        if (green_text) free(green_text);
        if (blue_text) free(blue_text);
        
        return NULL;
    }
    
    printf("\nDecoded parts:\n");
    printf("Red: %s\n", red_text);
    printf("Green: %s\n", green_text);
    printf("Blue: %s\n", blue_text);
    
    // Объединяем части
    char* combined = combine_parts(red_text, green_text, blue_text);
    if (!combined) {
        fprintf(stderr, "Failed to combine parts\n");
        
        free_image(image, height);
        free(red_text);
        free(green_text);
        free(blue_text);
        
        return NULL;
    }
    
    printf("Combined: %s\n", combined);
    
    crypt_data(combined, password, strlen(combined));
    printf("Final decoded string: %s\n", combined);
    
    // Освобождаем память
    free_image(image, height);
    free(red_text);
    free(green_text);
    free(blue_text);
    
    return combined;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
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
    if (result) {
        printf("\n✅ Successfully decoded!\n");
        printf("Final result: %s\n", result);
        free(result);
        return 0;
    } else {
        printf("\n❌ Failed to decode QR image\n");
        return 1;
    }
}
