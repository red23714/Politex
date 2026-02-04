// #define _CRT_SECURE_NO_WARNINGS

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>

// // написать функцию на вход 2 строки на выходе новая строка которая формуриуется из двух по следующим правилам: 
// // в первой строке удаляються все согласные которые есть во второй строке, 
// // а во второй строке удваиваются все главсные которых нет в первой строке после чего строки склеиваются

// void compare(char *str1, char *str2, char* ans)
// {
//     int len_1 = strlen(str1);
//     int len_2 = strlen(str2);

//     int idx = 0;

//     char str2_consonants[256];
//     char str1_vowels[256];

//     char vowels[] = "AaEeIiOoUu";

//     for (int i = 0; i < len_2; i++)
//     {
//         if (!strchr(vowels, str2[i]))
//         {
//             str2_consonants[str2[i]] = '1';
//         }
//     }
    
//     for (int i = 0; i < len_1; i++)
//     {
//         if (strchr(vowels, str1[i]))
//         {
//             str1_vowels[str1[i]] = '1';
//         }
//         else if (str2_consonants[str1[i]] == '1')
//         {
//             continue;
//         }
//         ans[idx] = str1[i];
//         idx++;
//     }

//     for (int i = 0; i < len_2; i++)
//     {
//         if (strchr(vowels, str2[i]) && str1_vowels[str2[i]] != '1')
//         {
//             ans[idx] = str2[i];
//             idx++;
//         }
//         ans[idx] = str2[i];
//         idx++;
//     }
// }

// int main(int argc, char* argv[])
// {
//     char first[32] = {'0'}, second[32] = {'0'}, ans[64] = {'0'};
    
//     scanf("%s", first);
//     scanf("%s", second);

//     compare(first, second, ans);

//     printf("%s", ans);

//     return 0;
// }

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// написать функцию на вход 2 строки на выходе новая строка которая формуриуется из двух по следующим правилам: 
// в первой строке удаляються все согласные которые есть во второй строке, 
// а во второй строке удваиваются все главсные которых нет в первой строке после чего строки склеиваются

int is_vowel(char c)
{
    char vowels[] = "AaEeIiOoUu";
    char russians[] = { -96, -128, -91, -123, -15, -16, -88, -120, -82, -114, -29, -109, -21, -101, -19, -99, -18, -98, -17, -97 };
    for (int i = 0; i < 20; i++)
    {
        if (c == vowels[i] || c == russians[i])
        {
            return 1;
        }
    }
    return 0;
}

void compare(char* str1, char* str2, char* ans)
{
    int len_1 = strlen(str1);
    int len_2 = strlen(str2);

    int idx = 0;

    for (int i = 0; i < 200; i++)
    {
        ans[i] = '0';
    }

    char str2_consonants[256] = {'0'};
    char str1_vowels[256] = {'0'};

    for (int i = 0; i < len_2; i++)
    {
        if (!is_vowel(str2[i]))
        {
            str2_consonants[str2[i] + 128] = '1';
        }
    }

    for (int i = 0; i < len_1; i++)
    {
        if (is_vowel(str1[i]))
        {
            str1_vowels[str1[i] + 128] = '1';
        }
        else if (str2_consonants[str1[i] + 128] == '1')
        {
            continue;
        }
        ans[idx] = str1[i];
        idx++;
    }

    for (int i = 0; i < len_2; i++)
    {
        if (is_vowel(str2[i]) && str1_vowels[str2[i] + 128] != '1')
        {
            ans[idx] = str2[i];
            idx++;
        }
        ans[idx] = str2[i];
        idx++;
    }
    ans[idx] = '\0';
}

int main(int argc, char* argv[])
{
    char first[100] = { '0' }, second[100] = { '0' }, ans[200] = { '0' };

    while(1)
    {
        scanf("%s", first);
        scanf("%s", second);

        compare(first, second, ans);

        printf("%s\n", ans);
    }

    return 0;
}