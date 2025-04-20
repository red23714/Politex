#include "stdio.h"
#include "hash.h"
#include "sys/stat.h"

char *read_file(const char *filename, size_t *size)
{
    struct stat st;
    stat(filename, &st);

    *size = st.st_size;

    FILE *file = fopen(filename, "rb");

    char *buffer = malloc(*size);
    size_t read = 0;

    while (read < *size)
    {
        read += fread(buffer + read, 1, *size - read, file);
    }

    fclose(file);
    return buffer;
}

char *replace_strings(char *str, size_t size)
{
    HashTable *dict = create_table();
    char *result = malloc(size * 2);
    int result_pos = 0;

    int i = 0;
    while (i < size)
    {
        char current_seq[256];
        int seq_len = 0;
        int found_code = -1;
        int last_found_code = -1;
        int last_found_len = 0;

        while (i + seq_len < size)
        {
            current_seq[seq_len] = str[i + seq_len];
            seq_len++;
            current_seq[seq_len] = '\0';

            int code;
            if (search(dict, current_seq, seq_len, &code))
            {
                last_found_code = code;
                last_found_len = seq_len;
            }
            else
            {
                break;
            }
        }

        if (last_found_code != -1)
        {
            char code_str[20];
            sprintf(code_str, "[%d]", last_found_code);
            strcpy(result + result_pos, code_str);
            result_pos += strlen(code_str);

            i += last_found_len;

            if (i < size)
            {
                char new_seq[256];
                memcpy(new_seq, str + i - last_found_len, last_found_len + 1);
                insert(dict, new_seq, last_found_len + 1, dict->count + 1);
            }
        }
        else
        {
            char single_char[2] = {str[i], '\0'};
            insert(dict, single_char, 1, dict->count + 1);

            result[result_pos++] = str[i++];
        }
    }

    result[result_pos] = '\0';
    free_table(dict);
    return realloc(result, result_pos + 1);
}

int main()
{
    const char *filename = "file.txt";
    size_t file_size;
    char *file_content = read_file(filename, &file_size);

    char *output = replace_strings(file_content, file_size);

    printf("Result: %s %d \n", output, strlen(output));

    return 0;
}