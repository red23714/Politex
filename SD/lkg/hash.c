#include "hash.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Простая хеш-функция (можете использовать более сложную при необходимости)
size_t hash(uint64_t key, size_t size) {
    uint32_t key_part = (uint32_t)key;
    return key_part % size;
}

HashTable* create_hashtable(size_t size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    
    ht->size = size;
    ht->table = (Node**)calloc(size, sizeof(Node*));
    
    return ht;
}

bool insert_hashtable(HashTable* ht, uint64_t key) {
    size_t index = hash(key, ht->size);

    Node* current = ht->table[index];
    while (current != NULL) {
        if (current->key == key) {
            // Элемент уже существует - коллизия
            return true;
        }
        current = current->next;
    }

    // Элемент не найден, добавляем новый узел
    Node* new_node = (Node*)malloc(sizeof(Node));
   
    new_node->key = key;
    new_node->next = ht->table[index];
    ht->table[index] = new_node;

    return false; // Элемент успешно добавлен, коллизии не было
}

void free_hashtable(HashTable* ht) {
    if (ht == NULL) {
        return;
    }
    for (size_t i = 0; i < ht->size; ++i) {
        Node* current = ht->table[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}
