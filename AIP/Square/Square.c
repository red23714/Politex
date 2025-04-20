#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>

int check_sqrt(unsigned long long n)
{
    unsigned long long root = (pow((double)n, 0.5) + 0.5);
    return root * root == n;
}

int return_max(unsigned long long arr_in[])
{
    unsigned long long max = 0;
    int idx = -1;
    for (int i = 0; i < 20; i++)
    {
        if (max < arr_in[i])
        {
            idx = i;
            max = arr_in[i];
        }
    }

    return idx;
}

int main(int argc, char* argv[])
{
    int N;
    unsigned long long arr[20] = { 0 };
    unsigned long long ans_buff[20] = { 0 };

    scanf("%d", &N);

    int zero_pos = -1;

    for (int i = 0; i < N; i++)
    {
        scanf("%llu", &arr[i]);
        if (arr[i] == 0 && zero_pos == -1)
        {
            zero_pos = i;
        }

        if (check_sqrt(arr[i]) && arr[i] != 0)
        {
            ans_buff[i] = arr[i];
        }
    }

    int idx = return_max(ans_buff);

    if (idx != -1 && arr[return_max(ans_buff)] > 0)
    {
        printf("%d", return_max(ans_buff));
    }
    else if(zero_pos != -1)
    {
        printf("%d", zero_pos);
    }
    else
    {
        printf("-1");
    }

    return 0;
}

//#define _CRT_SECURE_NO_WARNINGS
//#include <stdio.h>
//#include <string.h>
//
//void sumstrings(char* s3, char* s4, char* ss)
//{
//    ss[0] = '\0';
//    int a = 0;
//    char s5[33] = "";
//    char s6[33] = "";
//    size_t l1 = strlen(s3);
//    size_t l2 = strlen(s4);
//    strcpy(s5, s3);
//    strcpy(s6, s4);
//    s5[l1] = '\0';
//    s6[l2] = '\0';
//    for (int i = 0; i < l1; i++)
//    {
//        while (*s3 == *s4)
//        {
//            a++;
//            *s3++;
//            *s4++;
//            i++;
//            if (*s3 != *s4 && i < l1)
//            {
//                a = 0;
//                strcpy(s4, s6);
//                s4[l2] = '\0';
//            }
//        }
//        *s3++;
//    }
//    if (a < l2)
//    {
//        strcat(ss, s5);
//        strcat(ss, s4);
//    }
//    else
//    {
//        strcat(ss, s5);
//    }
//    ss[strlen(ss)] = '\0';
//}
//
//int main()
//{
//    int N;
//    char s1[33] = "";
//    char s2[33] = "";
//    char s[33][66];
//    scanf("%d", &N);
//    for (int i = 0; i < N; i++)
//    {
//        scanf("%s", s1);
//        scanf("%s", s2);
//        sumstrings(s1, s2, s[i]);
//    }
//
//    for (int i = 0; i < N; i++)
//    {
//        printf("%s\n", s[i]);
//    }
//
//    return 0;
//}