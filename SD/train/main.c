#include <stdio.h>
#include "list.h"

#define TRAIN_SIZE 100

char train_on = '1';
char train_off = '0';

int main()
{
    list train;
    char train_str[TRAIN_SIZE] = "";
    int index = 0;
    int count = 0;

    scanf("%s", train_str);
    scanf("%d", &index);

    list_init(&train, train_str);

    set_item(&train, index, train_on);
    count++;

    while (1)
    {
        index += count;
        set_item(&train, index, train_off);
        index -= count;
        if(get_item(&train, index)->data == train_off) break;
        count++;
    }
    
    printf("%d \n", count);

    return 0;
}