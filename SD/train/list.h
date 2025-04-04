#pragma once

#include <stdlib.h>

typedef struct element
{
    struct element *next;
    char data;
}element;

typedef struct list
{
    element *head;
    element *tail;
}list;

void add_to_begin(list *list, char data)
{
    element *new_element = (element*) malloc(sizeof(element));
    new_element->data = data;

    if (list->head == NULL) 
    {
        new_element->next = new_element;
        list->head = new_element;
        list->tail = new_element;
    } 
    else 
    {
        new_element->next = list->head;
        list->head = new_element;
        list->tail->next = list->head;
    }
}

void add_to_end(list *list, char data)
{
    if (list->head == NULL) 
    {
        add_to_begin(list, data);
        return;
    }

    element *new_element = (element*) malloc(sizeof(element));

    new_element->data = data;
    new_element->next = list->head;

    list->tail->next = new_element;

    list->tail = new_element;
}

void list_init(list *list, char *str)
{
    list->head = NULL;
    list->tail = NULL;

    int i = 0;
    while (str[i] != '\0')
    {
        add_to_end(list, str[i]);
        i++;
    }
}

element* get_item(list* list, int index)
{
    int i = 0;
    element *current = list->head;

    while (i != index)
    {
        current = current->next;

        if(index > 0) i++;
        else i--;
    }
    
    return current;
}

void set_item(list* list, int index, char data)
{
    element *el = get_item(list, index);
    el->data = data;
}

void print_list(list* list)
{
    element *current = list->head;
    printf("%c", current->data);

    while(current != list->tail)
    {
        current = current->next;
        printf("%c", current->data);
    }

    printf("\n");
}
