#include <stdio.h>
#include <string.h>

char train_on = '1';
char train_off = '0';

int main()
{
    char train[100] = "";
    int index = 0;
    int count = 0;

    scanf("%s", train);

    train[index] = train_on;
    count++;

    while (1)
    {
        index += count;
        if(index == strlen(train)) index = 0;
        train[index] = train_off;
        index -= count;
        if(index < 0) index = 0; 
        if(train[index] == train_off) break;
        count++;
    }
    
    printf("%d", count);

    return 0;
}