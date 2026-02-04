#ifndef LZW_H
#define LZW_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t lzw_compress_mem(const unsigned char* input, size_t input_size,
						unsigned char** output);

size_t lzw_decompress_mem(const unsigned char* input, size_t input_size,
						  unsigned char** output);

#endif
