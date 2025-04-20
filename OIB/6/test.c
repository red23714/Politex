#include <stdio.h>
#include <stdio.h>
#include <string.h>

#define HELLO 12

struct test
{
    int hello;
};

void br ()
{
    struct test hi;
}

typedef struct
{
    int a;
}hero;

void br2 ()
{
    struct test hi;
}

int main1()
{
    hero man;
    int te = 'c';
    unsigned int a = 10.f; // var
    const int hero = a;
    struct test new;
    long long d = 1000;
    char b[100] = "hello /*hello*/ br"; // char " asfsdf"
    size_t i;

    /*
    hello
    */

    br();

    // for (size_t i = 0; i < 10; i++)
    // {
    //     while (0)
    //     {
            
    //     }
        
    // }
    

    printf("%d %s", a, b);

    return 0;
}

int check_password(const char * pass, const char * true_pass)
{
    int res = strncmp(pass, true_pass, strlen(pass));
    return res == 0;
}

int main(int argc, char const *argv[])
{
    int is_login = 0;
    char pass[50];
    char true_pass[50];

    FILE *fptr;
    // opening the file in read mode
    fptr = fopen("pass.txt", "r");

    // checking if the file is opened successfully
    if (fptr == NULL)
    {
        printf("The file is not opened. The program will now exit.");
        return 0;
    }

    while (fgets(true_pass, 50, fptr) != NULL);

    // Closing the file using fclose()
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
            main1();
        }
        else
        {
            printf("You are not log in\nTry again:\n");
            scanf("%s", pass);
        }
    }

    return 0;
}
