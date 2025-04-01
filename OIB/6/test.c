#include "stdio.h"

struct test
{
    int hello;
};


int main()
{
    struct test new;
    int a = 10; // var
    long long d = 1000;
    char b[100] = "hello /*hello*/"; // char " asfsdf"

    /*
    hello
    */

    printf("%d %c", a, b);
}