#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

void lzw_compress(FILE *input, FILE *output) {
    HashTable *dictionary = create_table();
    
    for (int i = 0; i < 256; i++) {
        char c = (char)i;
        insert(dictionary, &c, 1, i);
    }
    
    int next_code = 256;
    char buffer[4096];
    size_t buffer_pos = 0;
    char current_phrase[256];
    size_t phrase_len = 0;
    
    while (!feof(input)) {
        size_t bytes_read = fread(buffer + buffer_pos, 1, sizeof(buffer) - buffer_pos, input);
        if (bytes_read == 0 && buffer_pos == 0) break;
        
        size_t total_bytes = buffer_pos + bytes_read;
        size_t processed = 0;
        
        while (processed < total_bytes) {
            current_phrase[phrase_len] = buffer[processed];
            phrase_len++;
            
            int value;
            if (!search(dictionary, current_phrase, phrase_len, &value)) {
                if (phrase_len > 1) {
                    search(dictionary, current_phrase, phrase_len - 1, &value);
                    fwrite(&value, sizeof(int), 1, output);
                }
                
                if (next_code < 65536) {
                    insert(dictionary, current_phrase, phrase_len, next_code++);
                }
                
                current_phrase[0] = buffer[processed];
                phrase_len = 1;
            }
            
            processed++;
        }
        
        buffer_pos = total_bytes - processed;
        if (buffer_pos > 0) {
            memmove(buffer, buffer + processed, buffer_pos);
        }
    }
    
    if (phrase_len > 0) {
        int value;
        search(dictionary, current_phrase, phrase_len, &value);
        fwrite(&value, sizeof(int), 1, output);
    }
    
    free_table(dictionary);
}

void lzw_decompress(FILE *input, FILE *output) {
    char **reverse_dict = (char **)malloc(65536 * sizeof(char *));
    size_t *reverse_dict_len = (size_t *)malloc(65536 * sizeof(size_t));
    
    for (int i = 0; i < 256; i++) {
        reverse_dict[i] = (char *)malloc(1);
        reverse_dict[i][0] = (char)i;
        reverse_dict_len[i] = 1;
    }
    
    int next_code = 256;
    int old_code, new_code;
    
    if (fread(&old_code, sizeof(int), 1, input) != 1) {
        for (int i = 0; i < 256; i++) free(reverse_dict[i]);
        free(reverse_dict);
        free(reverse_dict_len);
        return;
    }
    
    fwrite(reverse_dict[old_code], 1, reverse_dict_len[old_code], output);
    
    while (fread(&new_code, sizeof(int), 1, input) == 1) {
        char *string;
        size_t string_len;
        
        if (new_code < next_code) {
            string = reverse_dict[new_code];
            string_len = reverse_dict_len[new_code];
        } else {
            string = (char *)malloc(reverse_dict_len[old_code] + 1);
            memcpy(string, reverse_dict[old_code], reverse_dict_len[old_code]);
            string[reverse_dict_len[old_code]] = reverse_dict[old_code][0];
            string_len = reverse_dict_len[old_code] + 1;
        }
        
        fwrite(string, 1, string_len, output);
        
        if (next_code < 65536) {
            reverse_dict[next_code] = (char *)malloc(reverse_dict_len[old_code] + 1);
            memcpy(reverse_dict[next_code], reverse_dict[old_code], reverse_dict_len[old_code]);
            reverse_dict[next_code][reverse_dict_len[old_code]] = string[0];
            reverse_dict_len[next_code] = reverse_dict_len[old_code] + 1;
            next_code++;
        }
        
        old_code = new_code;
    }
    
    for (int i = 0; i < next_code; i++) {
        free(reverse_dict[i]);
    }
    free(reverse_dict);
    free(reverse_dict_len);
}
