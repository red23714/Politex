#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char* argv[])
{
    int i = 1;
    long double current_sum = 0;
    long double num = -7340.209773806239354599;
    long double sum_old = 0.2;

    int8_t a;

    double x;

    scanf("%lf", &x);
    current_sum = -7340.209773806239354599;

    printf("%d %.18Lf \n", i, current_sum);
    i++;

    //матрица типа long n * n напишите процедуру возвращающую такие i и j Aij = Асреднее

    while (current_sum != sum_old)
    {
        sum_old = current_sum;
        a = (int)(i / 2) % 2 == 0 ? -1 : 1;
        num = a * fabs(num) * x * x / (i + 1)  / 2;
        current_sum += num;
        i++;
        printf("%d %.18Lf %.18Lf \n", i, num, current_sum);
    }
    

    return 0;
}