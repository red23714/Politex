#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define _CRT_SECURE_NO_WARNINGS

void return_mid(long int A[20][20], int N, int M, long long sum, int* ansi, int* ansj)
{
    long long int mid_sub = INT64_MAX, mid_p = 0, mid = sum / (N * M);
    int ai, aj;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (abs(A[i][j] - mid) < mid_sub)
            {
                *ansi = i;
                *ansj = j;
                mid_sub = abs(A[i][j] - mid);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    long int A[20][20];
    long long int sum = 0, mid;
    int N, M;
    int ansi;
    int ansj;

    scanf("%d", &N);
    scanf("%d", &M);
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            scanf("%ld", &A[i][j]);
            sum += A[i][j];
        }
    }

    return_mid(A, N, M, sum, &ansi, &ansj);

    printf("%d %d", ansi, ansj);

    return 0;
}