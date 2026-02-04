#ifndef _PREFIX_TREE_H
#define _PREFIX_TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node 
{
   struct node* next[63];
   char value;
   bool is_value;
   int count_of_values;
}node;

typedef struct 
{
    node* start;
    int word_counts;
}prefix_tree;

int char_to_index(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a';          // 0-25
    } else if (ch >= 'A' && ch <= 'Z') {
        return 26 + (ch - 'A');   // 26-51
    } else if (ch >= '0' && ch <= '9') {
        return 52 + (ch - '0');    // 52-61
    } else if (ch == '_') {
        return 62;                 // 62
    } else {
        return -1; // Недопустимый символ
    }
}

int has_child(node* nd, char ch)
{
    int pos = char_to_index(ch);
    if(!nd->next[pos]) return false;
    return true;
}

void add_child(node* nd, char ch)
{
    int pos = char_to_index(ch);
    if(nd->next[pos]) return;

    node *new_node = (node*)malloc(sizeof(node));
    new_node->count_of_values = 0;
    new_node->value = ch;
    new_node->is_value = false;

    // Обнуляем все указатели next
    for (int i = 0; i < 63; i++) {
        new_node->next[i] = NULL;
    }
    
    nd->next[pos] = new_node;
}

void print_descendants(node *nd, char* start, char* str)
{
    if(nd->is_value) printf("%s%s %d\n", start, str, nd->count_of_values);
    for(int i = 0; i < 63; i++)
    {
        if(nd->next[i]) 
        {
            strncat(str, &nd->next[i]->value, 1);
            print_descendants(nd->next[i], start,  str);
            str[strlen(str) - 1] = '\0';
        }
    }
}

node* find_node(prefix_tree *pt, char* str, bool is_insert)
{
    node* nd = pt->start;
    for (int i = 0; i < strlen(str); i++) 
    {
        if(!has_child(nd, str[i]))
        {
            if(is_insert) 
            {
                add_child(nd, str[i]);
            }
            else return nullptr;
        }
        int pos = char_to_index(str[i]);
        nd = nd->next[pos];
    }

    return nd;
}

void init_tree(prefix_tree *pt)
{
    node *root_node = (node*)malloc(sizeof(node));
    root_node->value = '\0';
    root_node->is_value = false;
    root_node->count_of_values = 0;

   // Обнуляем все указатели next
    for (int i = 0; i < 63; i++) {
        root_node->next[i] = NULL;
    }

    pt->start = root_node;
    pt->word_counts = 0;
}

void print_node(node* nd, int level) {
    if (!nd) return;

    // Отступ в зависимости от уровня
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Выводим текущий узел
    printf("'%c'", nd->value);
    if (nd->is_value) {
        printf(" (end, count=%d)", nd->count_of_values);
    }
    printf("\n");

    // Рекурсивно выводим детей
    for (int i = 0; i < 63; i++) {
        if (nd->next[i]) {
            print_node(nd->next[i], level + 1);
        }
    }
}

void print_tree(prefix_tree *pt) {
    if (!pt || !pt->start) {
        printf("(empty tree)\n");
        return;
    }

    printf("Prefix Tree (root):\n");
    for (int i = 0; i < 63; i++) {
        if (pt->start->next[i]) {
            print_node(pt->start->next[i], 1);
        }
    }
}

void trie_insert(prefix_tree *pt, char* str)
{
    node* nd = find_node(pt, str, true);

    pt->word_counts++;
    nd->count_of_values++;
    nd->is_value = true;
}

int trie_lookup(prefix_tree *pt, char* str)
{
    node* nd = find_node(pt, str, false);
    
    if(!nd) return 0;

    if(nd->is_value) return nd->count_of_values;
    else return 0;
}

void trie_search(prefix_tree *pt, char* prefix)
{
    node* nd = find_node(pt, prefix, false);

    if(!nd) 
    {
        printf("No value\n");
        return;
    }
    char test[100] = "\0";
    print_descendants(nd, prefix, test);
}

#endif
