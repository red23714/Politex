#include <stdio.h>
#include <malloc.h>

#define ll long long

ll give_n_sequence(int n) 
{
    ll *previous = (ll*)malloc((n + 1) * sizeof(ll));
    previous[0] = 1;

    int i3 = 0;
    int i5 = 0;
    int i7 = 0;

    for (int i = 1; i <= n; i++) 
    {
        ll next3 = previous[i3] * 3;
        ll next5 = previous[i5] * 5;
        ll next7 = previous[i7] * 7;

        ll next = next3;
        if (next5 < next) next = next5;
        if (next7 < next) next = next7;

        previous[i] = next;

        if (next == next3) i3++;
        if (next == next5) i5++;
        if (next == next7) i7++;

        printf("%lld\n", previous[i]);
    }

    return previous[n];
}

int main() 
{
    int n;
    
    // scanf("%d", &n);

    printf("%lld\n", give_n_sequence(4));

    return 0;
}