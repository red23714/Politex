#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_SIZE 4096    
#define LOOKAHEAD_BUFFER 256 
#define BUFFER_SIZE (WINDOW_SIZE + LOOKAHEAD_BUFFER)

typedef struct {
    unsigned short offset;   
    unsigned char length;    
    unsigned char next_char; 
}LZ77Token;

void lz77_compress(FILE* input, FILE* output) {
    unsigned char window[BUFFER_SIZE];
    unsigned int window_fill = 0;
    unsigned int lookahead_pos = 0;
    unsigned int search_pos;
    int bytes_read;

    bytes_read = fread(window, 1, BUFFER_SIZE, input);
    if (bytes_read <= 0) return;

    while (lookahead_pos < bytes_read) {
        LZ77Token token = {0, 0, window[lookahead_pos]};
        unsigned int max_search = (lookahead_pos < WINDOW_SIZE) ? lookahead_pos : WINDOW_SIZE;
        unsigned int max_length = (bytes_read - lookahead_pos) < LOOKAHEAD_BUFFER ? 
                                (bytes_read - lookahead_pos) : LOOKAHEAD_BUFFER;

        for (search_pos = 1; search_pos <= max_search; search_pos++) {
            unsigned int match_len = 0;
            while (match_len < max_length && 
                   window[lookahead_pos - search_pos + match_len] == window[lookahead_pos + match_len]) {
                match_len++;
            }

            if (match_len > token.length) {
                token.offset = search_pos;
                token.length = match_len;
                if (lookahead_pos + match_len < bytes_read) {
                    token.next_char = window[lookahead_pos + match_len];
                } else {
                    token.next_char = 0;
                }
            }
        }

        fwrite(&token, sizeof(LZ77Token), 1, output);

        unsigned int shift = token.length + 1;
        lookahead_pos += shift;

        if (lookahead_pos >= WINDOW_SIZE) {
            unsigned int remaining = bytes_read - lookahead_pos;
            memmove(window, window + lookahead_pos - WINDOW_SIZE, WINDOW_SIZE + remaining);
            
            unsigned int read_pos = WINDOW_SIZE + remaining;
            unsigned int to_read = BUFFER_SIZE - read_pos;
            if (to_read > 0) {
                int new_bytes = fread(window + read_pos, 1, to_read, input);
                bytes_read = read_pos + new_bytes;
            } else {
                bytes_read = read_pos;
            }
            
            lookahead_pos = WINDOW_SIZE;
        }
    }
}

void lz77_decompress(FILE* input, FILE* output) {
    unsigned char window[WINDOW_SIZE] = {0};  
    unsigned int window_pos = 0;              
    LZ77Token token;
    
    while (fread(&token, sizeof(LZ77Token), 1, input) == 1) {
        if (token.length > 0) {
            unsigned int start_pos = (window_pos - token.offset) % WINDOW_SIZE;
            
            for (int i = 0; i < token.length; i++) {
                unsigned char c = window[(start_pos + i) % WINDOW_SIZE];
                fputc(c, output);
                window[window_pos] = c;
                window_pos = (window_pos + 1) % WINDOW_SIZE;
            }
        }
        
        if (token.next_char != 0) {
            fputc(token.next_char, output);
            window[window_pos] = token.next_char;
            window_pos = (window_pos + 1) % WINDOW_SIZE;
        }
    }
}

