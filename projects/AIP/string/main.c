#include <stdio.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS

void concat(char a[], char b[], char ans[])
{
    int len_a = strlen(a);
    int len_b = strlen(b);
    int idx = -1, cnt = 0;

    for (int i = 0; i < (len_a + len_b); i++)
    {
        if (i < len_a)
        {
            if (a[i] == b[cnt])
            {
                cnt += 1;
            }
            else
            {
                idx = -1;
                cnt = 0;
            }
        }

        if (cnt != 0 && idx == -1)
        {
            idx = len_a - i;
        }
        
        
        if (i < len_a)
        {
            ans[i] = a[i];
        }
        else
        {
            if (idx == -1)
            {
                ans[i] = b[i - len_a];
            }
            else
            {
                idx++;
                ans[i] = b[idx];
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int n;
    char first[32], second[32], ans[64];
    
    scanf("%d", &n);
    scanf("%s", first);
    scanf("%s", second);

    concat(first, second, ans);
    printf("%s", ans);

    return 0;
}