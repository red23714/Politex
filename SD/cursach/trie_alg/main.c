#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qr_encoder.h"
#include "lzw.h"

void print_usage() {
    printf("Usage:\n");
    printf("  Compression:   ./main -c <input_file> <output_file> --password <password>\n");
    printf("  Decompression: ./main -d <input_file> <output_file> --password <password>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 6 || strcmp(argv[4], "--password") != 0) {
        print_usage();
        return 1;
    }

    char mode = argv[1][1]; // -c или -d
    char *input_file = argv[2];
    char *output_file = argv[3];
    char *password = argv[5];

    if (mode == 'c') {
        // Чтение входного файла
        FILE *input = fopen(input_file, "rb");
        if (!input) {
            perror("Error opening input file");
            return 1;
        }

        fseek(input, 0, SEEK_END);
        long file_size = ftell(input);
        fseek(input, 0, SEEK_SET);

        char *file_content = malloc(file_size + 1);
        if (!file_content) {
            perror("Memory allocation error");
            fclose(input);
            return 1;
        }

        fread(file_content, 1, file_size, input);
        file_content[file_size] = '\0';
        fclose(input);
        
        // Генерация QR кода
        if (!generate_qr(file_content, password)) {
            printf("Error generating QR code\n");
            free(file_content);
            return 1;
        }

        // Сжатие PPM файла с помощью LZ77
        FILE* qr_input = fopen("qr.ppm", "rb");

        FILE* compressed_output = fopen(output_file, "wb");
       
        // Сжимаем данные
        lzw_compress(qr_input, compressed_output);

        fclose(qr_input);
        fclose(compressed_output);
        free(file_content);

        printf("Compression completed. Output: %s\n", output_file);

    } else if (mode == 'd') {
        // Декомпрессия
        FILE *compressed_input = fopen(input_file, "rb"); 

        // Декомпрессия LZ77 во временный файл
        FILE *compressed_output = fopen(output_file, "wb"); 

        lzw_decompress(compressed_input, compressed_output);
        fclose(compressed_input);

        printf("Decompression completed. Output: %s\n", output_file);

    } else {
        print_usage();
        return 1;
    }

    return 0;
}
