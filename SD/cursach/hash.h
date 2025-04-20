#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 10
#define LOAD_FACTOR 0.75

typedef struct Node
{
    char *data;    // Массив символов (не строка - может содержать '\0')
    size_t length; // Длина массива
    int value;
    struct Node *next;
} Node;

typedef struct
{
    Node **buckets;
    int size;
    int count;
} HashTable;

// Хеш-функция для массива символов
unsigned int hash(const char *data, size_t length, int table_size)
{
    unsigned int h = 0;
    for (size_t i = 0; i < length; i++)
    {
        h = h * 31 + data[i];
    }
    return h % table_size;
}

// Хеш-функция для одиночного символа
unsigned int hash_char(char c, int table_size)
{
    return (unsigned int)c % table_size;
}

HashTable *create_table()
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->size = INITIAL_SIZE;
    table->count = 0;
    table->buckets = (Node **)calloc(table->size, sizeof(Node *));
    return table;
}

Node *create_node(const char *data, size_t length, int value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = (char *)malloc(length);
    memcpy(node->data, data, length);
    node->length = length;
    node->value = value;
    node->next = NULL;
    return node;
}

void resize(HashTable *table)
{
    int new_size = table->size * 2;
    Node **new_buckets = (Node **)calloc(new_size, sizeof(Node *));

    for (int i = 0; i < table->size; i++)
    {
        Node *current = table->buckets[i];
        while (current != NULL)
        {
            Node *next = current->next;
            unsigned int new_index;

            if (current->length == 1)
            {
                new_index = hash_char(current->data[0], new_size);
            }
            else
            {
                new_index = hash(current->data, current->length, new_size);
            }

            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    free(table->buckets);
    table->buckets = new_buckets;
    table->size = new_size;
}

int compare_data(const char *a, size_t a_len, const char *b, size_t b_len)
{
    if (a_len != b_len)
        return 0;
    return memcmp(a, b, a_len) == 0;
}

void insert(HashTable *table, const char *data, size_t length, int value)
{
    // Проверка необходимости ресайза
    if ((float)table->count / table->size >= LOAD_FACTOR)
    {
        resize(table);
    }

    unsigned int index;
    if (length == 1)
    {
        index = hash_char(data[0], table->size);
    }
    else
    {
        index = hash(data, length, table->size);
    }

    // Проверка на существующий ключ
    Node *current = table->buckets[index];
    while (current != NULL)
    {
        if (compare_data(current->data, current->length, data, length))
        {
            current->value = value; // Обновление значения
            return;
        }
        current = current->next;
    }

    // Вставка нового элемента
    Node *new_node = create_node(data, length, value);
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->count++;
}

int search(HashTable *table, const char *data, size_t length, int *value)
{
    unsigned int index;
    if (length == 1)
    {
        index = hash_char(data[0], table->size);
    }
    else
    {
        index = hash(data, length, table->size);
    }

    Node *current = table->buckets[index];
    while (current != NULL)
    {
        if (compare_data(current->data, current->length, data, length))
        {
            *value = current->value;
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void free_table(HashTable *table)
{
    for (int i = 0; i < table->size; i++)
    {
        Node *current = table->buckets[i];
        while (current != NULL)
        {
            Node *next = current->next;
            free(current->data);
            free(current);
            current = next;
        }
    }
    free(table->buckets);
    free(table);
}