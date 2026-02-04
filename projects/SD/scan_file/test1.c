#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "prefix_tree.h"
#include "hash.h"

#define MAX_WORD_LENGTH 255

const char *c_keywords[] = 
{
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "while",
    "NULL", "true", "false", "bool" 
};

const char *c_modifiers[] = 
{
    "const", "static", "extern" 
};

bool is_keyword(const char *word) 
{
    for (int i = 0; c_keywords[i] != NULL; i++) 
    {
        if (strcmp(word, c_keywords[i]) == 0) 
        {
            return true;
        }
    }
    return false;
}

bool is_modifier(const char *word) 
{
    for (int i = 0; c_modifiers[i] != NULL; i++) 
    {
        if (strcmp(word, c_modifiers[i]) == 0) 
        {
            return true;
        }
    }
    return false;
}

typedef enum 
{
    SEARCH_TYPE,
    SEARCH_MODIFIER,
    SEARCH_POINTERS,
    SEARCH_NAME,
    SEARCH_FUNCTION,
    SKIP_SPACES,
    FINISHED
} ParserState;

bool is_valid_type_char(int c) 
{
    return isalpha(c) || c == '_' || (c == ':') || (c == '<') || (c == '>') || (c == ',');
}



bool is_valid_name_char(int c) 
{
    return isalnum(c) || c == '_';
}

void process_identifier(char* name, prefix_tree* pt, hash_table* ht) 
{     
    if (strlen(name) > 0 && !is_keyword(name)) 
    {
        trie_insert(pt, name);
        hash_insert(ht, name);
    }
}

void find_ids(FILE *file, prefix_tree *pt, hash_table *ht) 
{
    if (file == NULL || pt == NULL || ht == NULL) 
    {
        return;
    }

    ParserState state = SEARCH_TYPE;
    char current_word[MAX_WORD_LENGTH + 1] = {0};
    char type[MAX_WORD_LENGTH + 1] = {0};
    char name[MAX_WORD_LENGTH + 1] = {0};
    char pointers[MAX_WORD_LENGTH + 1] = {0};
    int word_pos = 0;
    int pointers_pos = 0;
    int paren_depth = 0;
    int c;
    bool has_type = false;
    bool has_name = false;
    bool is_function = false;
    bool has_modifier = false;

    while ((c = fgetc(file)) != EOF) 
    {
        switch (state) 
        {
            case SEARCH_TYPE:
                if (is_valid_type_char(c) && word_pos < MAX_WORD_LENGTH) 
                {
                    current_word[word_pos++] = c;
                }
                else if (isspace(c) && word_pos > 0) 
                {
                    current_word[word_pos] = '\0';
                    
                    if (is_modifier(current_word)) 
                    {
                        has_modifier = true;
                        state = SEARCH_MODIFIER;
                    }
                    else 
                    {
                        strncpy(type, current_word, MAX_WORD_LENGTH);
                        has_type = true;
                        state = SKIP_SPACES;
                    }
                    word_pos = 0;
                }
                else if (c == '(' || c == ';' || c == '=') 
                {
                    word_pos = 0;
                }
                else 
                {
                    if (word_pos > 0) 
                    {
                        word_pos = 0;
                    }
                }
                break;

            case SEARCH_MODIFIER:
                if (is_valid_type_char(c) && word_pos < MAX_WORD_LENGTH) 
                {
                    current_word[word_pos++] = c;
                }
                else if (isspace(c) && word_pos > 0) 
                {
                    current_word[word_pos] = '\0';
                    strncpy(type, current_word, MAX_WORD_LENGTH);
                    has_type = true;
                    word_pos = 0;
                    state = SKIP_SPACES;
                }
                break;

            case SKIP_SPACES:
                if (c == '*' && pointers_pos < MAX_WORD_LENGTH) 
                {
                    state = SEARCH_POINTERS;
                    pointers[pointers_pos++] = c;
                }
                else if (is_valid_name_char(c) && word_pos < MAX_WORD_LENGTH) 
                {
                    state = SEARCH_NAME;
                    current_word[word_pos++] = c;
                }
                else if (c == '(') 
                {
                    is_function = true;
                    state = SEARCH_FUNCTION;
                    paren_depth = 1;
                }
                else if (!isspace(c) && c != ';' && c != '=') 
                {
                    state = SEARCH_TYPE;
                    word_pos = 0;
                    pointers_pos = 0;
                    has_type = false;
                    has_modifier = false;
                }
                break;

            case SEARCH_POINTERS:
                if (c == '*' && pointers_pos < MAX_WORD_LENGTH) 
                {
                    pointers[pointers_pos++] = c;
                }
                else if (isspace(c)) 
                {
                    state = SKIP_SPACES;
                }
                else if (is_valid_name_char(c) && word_pos < MAX_WORD_LENGTH) 
                {
                    state = SEARCH_NAME;
                    current_word[word_pos++] = c;
                }
                else if (c == '(') 
                {
                    is_function = true;
                    state = SEARCH_FUNCTION;
                    paren_depth = 1;
                }
                else 
                {
                    state = SEARCH_TYPE;
                    word_pos = 0;
                    pointers_pos = 0;
                    has_type = false;
                    has_modifier = false;
                }
                break;

            case SEARCH_NAME:
                if (is_valid_name_char(c) && word_pos < MAX_WORD_LENGTH) 
                {
                    current_word[word_pos++] = c;
                }
                else if (c == '(') 
                {
                    is_function = true;
                    current_word[word_pos] = '\0';
                    strncpy(name, current_word, MAX_WORD_LENGTH);
                    has_name = true;
                    state = SEARCH_FUNCTION;
                    paren_depth = 1;
                }
                else if (isspace(c) || c == ';' || c == '=' || c == ',' || c == '[') 
                {
                    current_word[word_pos] = '\0';
                    strncpy(name, current_word, MAX_WORD_LENGTH);
                    has_name = true;
                    state = FINISHED;
                }
                else 
                {
                    state = SEARCH_TYPE;
                    word_pos = 0;
                    pointers_pos = 0;
                    has_type = false;
                    has_modifier = false;
                }
                break;

            case SEARCH_FUNCTION:
                if (c == '(') 
                {
                    paren_depth++;
                }
                else if (c == ')') 
                {
                    paren_depth--;
                }
                else if (c == ',' && paren_depth == 1) 
                {
                    
                    if (has_name) 
                    {
                        process_identifier(name, pt, ht);
                        has_name = false;
                    }
                    state = SEARCH_TYPE;
                    word_pos = 0;
                    pointers_pos = 0;
                    has_type = false;
                    has_modifier = false;
                    continue;
                }

                if (paren_depth == 0) 
                {
                    if (has_name) 
                    {
                        process_identifier(name, pt, ht);
                    }
                    state = SEARCH_TYPE;
                    word_pos = 0;
                    pointers_pos = 0;
                    has_type = false;
                    has_name = false;
                    is_function = false;
                    has_modifier = false;
                    type[0] = '\0';
                    name[0] = '\0';
                    pointers[0] = '\0';
                }
                break;

            case FINISHED:
                if (has_name) 
                {
                    process_identifier(name, pt, ht);
                }
                
                state = SEARCH_TYPE;
                word_pos = 0;
                pointers_pos = 0;
                has_type = false;
                has_name = false;
                is_function = false;
                has_modifier = false;
                type[0] = '\0';
                name[0] = '\0';
                pointers[0] = '\0';
                
                ungetc(c, file);
                break;
        }
    }

    if (has_name) 
    {
        process_identifier(name, pt, ht);
    }
}

int main() 
{
    FILE *f = fopen("test.c", "r"); 

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
