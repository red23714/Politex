#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qr_encoder.h"
#include "lzw.h"

void print_usage()
{
	printf("Usage:\n");
	printf("  Compression:   ./main -c <input_file> <output_file> --password "
		   "<password>\n");
	printf("  Decompression: ./main -d <input_file> <output_file> --password "
		   "<password>\n");
}

int main(int argc, char* argv[])
{
	if (argc != 6 || strcmp(argv[4], "--password") != 0)
	{
		print_usage();
		return 1;
	}

	char mode = argv[1][1];
	char* input_file = argv[2];
	char* output_file = argv[3];
	char* password = argv[5];

	if (mode == 'c')
	{
		// Читаем входной файл
		FILE* f = fopen(input_file, "rb");
		if (!f)
		{
			perror("input");
			return 1;
		}
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		char* buf = malloc(size + 1); // +1 для '\0'
		fread(buf, 1, size, f);
		buf[size] = '\0';
		fclose(f);

		// Генерируем QR PPM в памяти
		unsigned char* ppm_buf = NULL;
		size_t ppm_size = 0;
		if (!generate_qr_mem(buf, size, password, &ppm_buf, &ppm_size))
		{
			printf("QR generation failed\n");
			free(buf);
			return 1;
		}
		free(buf);

		// Сжимаем LZW в память
		unsigned char* compressed = NULL;
		size_t compressed_size =
			lzw_compress_mem(ppm_buf, ppm_size, &compressed);
		free(ppm_buf);

		// Сохраняем результат
		FILE* out = fopen(output_file, "wb");
		fwrite(compressed, 1, compressed_size, out);
		fclose(out);
		free(compressed);

		printf("Compression completed. Output: %s\n", output_file);
	}
	else if (mode == 'd')
	{
		// Читаем сжатый файл
		FILE* f = fopen(input_file, "rb");
		if (!f)
		{
			perror("input");
			return 1;
		}
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		unsigned char* compressed = malloc(size);
		fread(compressed, 1, size, f);
		fclose(f);

		// Декомпрессия в память
		unsigned char* decompressed = NULL;
		size_t decompressed_size =
			lzw_decompress_mem(compressed, size, &decompressed);
		free(compressed);

		// Сохраняем декомпрессированный PPM
		FILE* out = fopen(output_file, "wb");
		fwrite(decompressed, 1, decompressed_size, out);
		fclose(out);
		free(decompressed);

		printf("Decompression completed. Output: %s\n", output_file);
	}
	else
	{
		print_usage();
		return 1;
	}
	return 0;
}
