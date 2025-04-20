#include <stdio.h>
#include <string.h>

#define Const_XOR 50

int check_password(const char * pass, char * true_pass)
{
    for (int i = 0; i < strlen(true_pass); i++)
    {
        true_pass[i] ^= Const_XOR;
    }

    int res = strncmp(pass, true_pass, strlen(pass));
    
    return res == 0 && strlen(pass) == strlen(true_pass);
}

int main(int argc, char const *argv[])
{
    int is_login = 0;
    char pass[50];
    char true_pass[50];

    FILE *fptr;
    fptr = fopen("passxor.txt", "r");

    if (fptr == NULL)
    {
        printf("The file is not opened. The program will now exit.");
        return 0;
    }

    while (fgets(true_pass, 50, fptr) != NULL);

    fclose(fptr);

    printf("Enter password: \n");
    scanf("%s", pass);

    while (1)
    {
        is_login = check_password(pass, true_pass);

        if (is_login)
        {
            char test[20];
            printf("\nYou are log in\nType something: \n");
            scanf("%s", test);
            printf("%s\n", test);
        }
        else
        {
            printf("\nYou are not log in\nTry again:\n");
            scanf("%s", pass);
        }
    }

    return 0;
}
