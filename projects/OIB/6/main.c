#include <stdio.h>
#include <string.h>

int check_password(const char * pass, const char * true_pass)
{
    return strncmp(pass, true_pass, strlen(pass)) == 0 && strlen(pass) == strlen(true_pass);
}

int main(int argc, char const *argv[])
{
    int is_login = 0;
    char pass[50];
    char true_pass[50];

    FILE *fptr;
    fptr = fopen("pass.txt", "r");

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
            printf("You are log in\nType something: \n");
            scanf("%s", test);
            printf("%s\n", test);
        }
        else
        {
            printf("You are not log in\nTry again:\n");
            scanf("%s", pass);
        }
    }

    return 0;
}
