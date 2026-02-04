#ifndef _HASH_H
#define _HASH_H

#include <string.h>
#include <stdlib.h>

#define INITIAL_SIZE 2      // Начальный размер таблицы
#define LOAD_FACTOR 0.7    // Максимальный коэффициент заполнения перед ресайзинг
                           
typedef struct hash_node
{
   struct hash_node* next;
   char* key;
   int value;
}hash_node;

typedef struct
{
    hash_node** table;
    int size;
    int count;
}hash_table;

int hash(char* value, int size)
{
    unsigned long hash = 5381;
    int c;

    while (c = *value++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % size;
}

void hash_init(hash_table* ht)
{
    ht->size = INITIAL_SIZE;
    ht->table = (hash_node**)calloc(ht->size,  sizeof(hash_node*));
}

void hash_resize(hash_table* ht, int new_size) {
    hash_node** new_table = (hash_node**)calloc(new_size, sizeof(hash_node*));
    
    for (int i = 0; i < ht->size; i++) {
        hash_node* current = ht->table[i];
        while (current != NULL) {
            hash_node* next = current->next;
            
            int new_index = hash(current->key, new_size);
            
            current->next = new_table[new_index];
            new_table[new_index] = current;
            
            current = next;
        }
    }
    
    free(ht->table);
    
    ht->table = new_table;
    ht->size = new_size;
}

void check_resize(hash_table* ht) {
    double load = (double)ht->count / ht->size;
    if (load >= LOAD_FACTOR) {
        hash_resize(ht, ht->size * INITIAL_SIZE);
    }
}

void hash_insert(hash_table* ht, char* value) {
    check_resize(ht);

    int index = hash(value, ht->size);
    
    hash_node* current = ht->table[index];
    while (current != NULL) {
        if (strcmp(current->key, value) == 0) {
            current->value++;
            return;  
        }
        current = current->next;
    }

    hash_node* new_node = (hash_node*)malloc(sizeof(hash_node));
    new_node->key = strdup(value);  
    new_node->value = 1;
    new_node->next = ht->table[index];  
    ht->table[index] = new_node;

    ht->count--;
}

int hash_search(hash_table* ht, char* value) {
    int index = hash(value, ht->size);
    hash_node* current = ht->table[index];
    
    while (current != NULL) {
        if (strcmp(current->key, value) == 0) {
            return current->value;  
        }
        current = current->next;
    }
    return 0;  
}

void hash_remove(hash_table* ht, char* value) {
    int index = hash(value, ht->size);
    hash_node* current = ht->table[index];
    hash_node* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, value) == 0) {
            if (prev == NULL) {
                ht->table[index] = current->next;  
            } else {
                prev->next = current->next;  
            }
            free(current->key);
            free(current);
            ht->count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void hash_free(hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        hash_node* current = ht->table[i];
        while (current != NULL) {
            hash_node* temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(ht->table);
    ht->size = 0;
}

#endif
