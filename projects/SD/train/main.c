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

    list_init(&train, train_str);

    element *start = train.head;

    start->data = train_on;
    count++;

    element *current = start;
    while (1)
    {
        index += count;
        for (int i = 0; i < index; i++)
        {
            current = move(1, current);
        }
        current->data = train_off;

        for (int i = index; i > index - count; i--)
        {
            current = move(0, current);
        }

        index -= count;

        if (start->data == train_off) break;

        count++;
    }
    
    printf("%d \n", count);

    return 0;
}
