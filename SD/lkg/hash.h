#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Определение структуры узла хеш-таблицы
typedef struct Node {
    uint64_t key;
    struct Node* next;
} Node;

// Определение структуры хеш-таблицы
typedef struct HashTable {
    Node** table;
    size_t size;
} HashTable;

// Инициализация хеш-таблицы
HashTable* create_hashtable(size_t size);

// Вставка элемента в хеш-таблицу. Возвращает true, если произошла коллизия, false в противном случае.
bool insert_hashtable(HashTable* ht, uint64_t key);

// Освобождение памяти, занимаемой хеш-таблицей
void free_hashtable(HashTable* ht);

#endif // HASHTABLE_H
