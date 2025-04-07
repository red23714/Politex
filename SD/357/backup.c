#include <stdio.h>
#include <malloc.h>

#define ll long long

ll min(ll x, ll y, ll z)
{
    if (x <= y && x <= z) return 0;
    if (y <= x && y <= z) return 1;
    return 2;
}

int valueinarray(long long val, long long *arr, size_t n) 
{
    for(size_t i = 0; i <= n; i++) {
        if(arr[i] == val)
            return 1;
    }
    return 0;
}

ll give_n_sequence(int n)
{
    ll answer;
    ll *previous[3];

    previous[0] = malloc(n * sizeof(ll));
    previous[1] = malloc(n * sizeof(ll));
    previous[2] = malloc(n * sizeof(ll));

    previous[0][0] = 3;
    previous[1][0] = 5;
    previous[2][0] = 7;

    int idx = 1;
    for (int i = 1; i <= n; i++)
    {
        if(!valueinarray(previous[(i - 1) % 3][(i - 1) / 3] * 3, previous[0], n)) 
        {
            previous[0][idx] = previous[(i - 1) % 3][(i - 1) / 3] * 3;
            previous[1][idx] = previous[(i - 1) % 3][(i - 1) / 3] * 5;
            previous[2][idx] = previous[(i - 1) % 3][(i - 1) / 3] * 7;
            idx++;
        }
    }

    for (int i = 0; i < idx; i++)
    {
        printf("%-5d", i);
    }
    printf("\n");
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < idx; i++)
        {
            printf("%-5lld", previous[j][i]);
        }
    printf("\n");
    }
    
    // int current_index = 0;
    // int i = 0;
    // while (i != n - 1)
    // {
    //     if (previous[0][current_index])
    //     {
            
    //     }
    // }
    

    return answer;
}

int main(int argc, char const *argv[])
{
    int n;
    
    // scanf("%d", &n);

    printf("%lld", give_n_sequence(50));

    return 0;
}