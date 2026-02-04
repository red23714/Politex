#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "prefix_tree.h"
#include "hash.h"
#include "remove_comment.h"

#define BUFFER_SIZE 1024

const char *ignore_keywords[] = {
    // Ключевые слова
    "typedef", "return", "else", "if", "for", "while", "switch", 
    "case", "default", "sizeof", "struct", "enum", "union",
    // Модификаторы хранения
    "static", "extern", "auto", "register", "inline", "restrict",
    // Квалификаторы типа
    "const", "volatile", 
    // Спецификаторы типа
    "signed", "unsigned", "short", "long"
};
const int num_ignore_keywords = sizeof(ignore_keywords) / sizeof(ignore_keywords[0]);

void find_all(const char *filename, char *search_str, prefix_tree *pt, hash_table *ht) {
    FILE *file = fopen(filename, "r");

    size_t search_len = strlen(search_str);
    int c;

    size_t pos = 0;
    int prev_char = 0;  
    int next_char;      

    while ((c = fgetc(file)) != EOF) {
        if (pos == 0) {
            next_char = fgetc(file);  
            ungetc(next_char, file); 
        }

        if (c == search_str[pos]) {
            pos++;

            if (pos == search_len) {
                if (search_len == 1) {
                    if ((prev_char == 0 || !isalpha(prev_char)) && 
                        (next_char == EOF || !isalpha(next_char))) {
                        hash_insert(ht, search_str);
                        trie_insert(pt, search_str);
                    }
                } else {
                    hash_insert(ht, search_str);
                    trie_insert(pt, search_str);
                }
                pos = 0;
            }
        } else {
            pos = (c == search_str[0]) ? 1 : 0;
        }

        prev_char = c;  
    }

    fclose(file);
}

bool is_valid_id_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

bool is_ignore_keyword(const char *str, size_t len) {
    for (int i = 0; i < num_ignore_keywords; i++) {
        size_t kw_len = strlen(ignore_keywords[i]);
        if (len == kw_len && strncmp(str, ignore_keywords[i], len) == 0) {
            return true;
        }
    }
    return false;
}

void find_ids(FILE *file, prefix_tree *pt, hash_table *ht) {
    char line[1024];
    int line_num = 0;
    bool in_function_params = false;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        char *ptr = line;
        
        if (*ptr == '#') continue;
        
        while (*ptr) {
            while (*ptr && isspace(*ptr)) ptr++;
            if (!*ptr) break;
            
            if (*ptr == '"') {
                ptr++;
                while (*ptr && *ptr != '"') ptr++;
                if (*ptr) ptr++;
                continue;
            }
            
            if (*ptr == '\'') {
                ptr++;
                while (*ptr && *ptr != '\'') ptr++;
                if (*ptr) ptr++;
                continue;
            }
            
            if (*ptr == '(' && !in_function_params) {
                in_function_params = true;
                ptr++;
                continue;
            }
            if (*ptr == ')' && in_function_params) {
                in_function_params = false;
                ptr++;
                continue;
            }
            
            char *word_start = ptr;
            while (*ptr && is_valid_id_char(*ptr)) ptr++;
            size_t word_len = ptr - word_start; 
            
            if (word_len > 0 && is_ignore_keyword(word_start, word_len)) {
                continue;
            }
            if (word_len > 0) {
                ptr = word_start;
            }
            
            char *type_start = ptr;
            while (*ptr && is_valid_id_char(*ptr)) ptr++;
            if (type_start == ptr) {
                ptr++;
                continue;
            }
            
            while (*ptr && isspace(*ptr)) ptr++;
            if (!*ptr) break;

            while (*ptr == '*') ptr++;

            while (*ptr && isspace(*ptr)) ptr++;
            if (!*ptr) break;
            
            char *name_start = ptr;
            while (*ptr && is_valid_id_char(*ptr)) ptr++;
            if (name_start == ptr) continue;
            
            bool valid_decl = false;
            if (in_function_params) {
                if (*ptr == ',' || *ptr == ')' || *ptr == '=' || isspace(*ptr)) {
                    valid_decl = true;
                }
            } else {
                if (*ptr == ';' || *ptr == '=' || *ptr == '(' || *ptr == '[' || *ptr == ',' || isspace(*ptr)) {
                    valid_decl = true;
                }
            }
            
            if (valid_decl) {
                char name[256];
                size_t name_len = ptr - name_start;
                if (name_len >= sizeof(name)) name_len = sizeof(name) - 1;
                strncpy(name, name_start, name_len);
                name[name_len] = '\0';
                
                if (isdigit(name[0])) continue;
                
                if(hash_search(ht, name) == 0)
                {
                    printf("Found ID: %s (line %d)\n", name, line_num);
                    find_all("test.c", name, pt, ht);
                }
            }
        }
    }
}

int main() 
{
    FILE *f_in = fopen("test.c", "r"); 
    FILE *f_out = fopen("test1.c", "w");

    remove_comments(f_in, f_out);
    fclose(f_in);
    fclose(f_out);

    FILE *f = fopen("test1.c", "r");

    hash_table ht;
    hash_init(&ht); 

    prefix_tree pt;
    init_tree(&pt);
    
    find_ids(f, &pt, &ht);
    fclose(f);

    while(true) 
    {
        int command;
        printf("Выберите цифру для действия:\n"
               "1 - поиск по дереву\n"
               "2 - общее количество слов\n"
               "3 - вывести дерево\n"
               "4 - поиск по хэш таблице\n"
               "5 - выход\n");
        
        scanf("%d", &command); 

        switch (command) 
        {
            case 1:
                printf("Введите название переменной/функции: ");
                char str[100];
                if (scanf("%s", str) == 1) 
                {
                    trie_search(&pt, str);
                }
                break;
            
            case 2:
                printf("%d\n", pt.word_counts);
                break;
            
            case 3:
                print_tree(&pt);
                break;
            
            case 4:
                printf("Введите название переменной/функции: ");
                char str_hash[100];
                if (scanf("%s", str_hash) == 1) 
                {
                    printf("%d\n", hash_search(&ht, str_hash));
                }
                break;
            
            case 5:
                hash_free(&ht);
                return 0;
            
            default:
                printf("Неизвестная команда\n");
        }
    }
}
