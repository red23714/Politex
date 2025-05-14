#include "stdio.h"
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

typedef struct 
{
    char *name;
    int pos;

    int lenght;
}identifier;

int main()
{
    FILE *f = fopen("test.c", "r");

    fseek(f, 0, SEEK_END);
    long long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *file = malloc(size);

    while (fgets(file, 256, f));
}
