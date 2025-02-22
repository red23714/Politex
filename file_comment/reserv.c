#include <stdio.h>

int main()
{
    FILE *infile = fopen("test.c", "r");
    FILE *outfile = fopen("test.wc", "w");

    int line = 1;

    char prev = 0, curr = 0;       // Хранение текущего и предыдущего символа

    while ((curr = fgetc(infile)) != EOF)
    {
        if (curr == '\n')
        {
            line++;
        }

        // Проверка на строку " или '
        if ((curr == '"' || curr == '\''))
        {
            char buf = 0;
            char string_delimiter = curr;
            int even_backslash = 0; // Четное ли колличество бэкслэшей

            if(prev != 0) fputc(prev, outfile);
            fputc(curr, outfile);

            while ((curr = fgetc(infile)) != EOF)
            {
                if (buf == '\\')
                {
                    if(curr == '\\') even_backslash = !even_backslash;        

                    if ((curr == '\'' || curr == '\"') && !even_backslash)
                    {
                        goto put_symbol;
                    }
                }
                else even_backslash = 0;

                if (curr == string_delimiter) break; 
                
                if (curr == '\n')
                {
                    line++;
                    if (buf != '\\' || (buf == '\\' && even_backslash) || string_delimiter == '\'')
                    {
                        break;
                    }
                }

                put_symbol:
                    buf = curr;
                    fputc(buf, outfile);
            }

            if(curr == EOF)
            {
                prev = 0;
                break;
            }

            prev = curr;
            continue;
        }

        // Проверка на однострочный комментарий (//)
        if (prev == '/' && curr == '/')
        {
            char buf = 0;
            while ((curr = fgetc(infile)) != EOF)
            {
                if (curr == '\n')
                {
                    line++;
                    if (buf != '\\')
                    {
                        break;
                    }
                }
                buf = curr;
            }

            if(curr == EOF)
            {
                prev = 0;
                break;
            }
            
            prev = 0;
            continue;
        }

        // Проверка на многострочный комментарий (/*)
        if (prev == '/' && curr == '*') 
        {
            char buf = 0;
            while ((curr = fgetc(infile)) != EOF)
            {
                if (buf == '*' && curr == '/')
                {
                    break;
                }
                if (curr == '\n')
                {
                    line++;
                }
                
                buf = curr;
            }
            
            if(curr == EOF)
            {
                prev = 0;
                break;
            }

            prev = 0;
            continue;
        }

        // Запись символа
        if (prev != 0)
        {            
            fputc(prev, outfile);
        }

        prev = curr;
    }

    // Проверяем, если последний символ не был записан
    if (prev != 0)
    {
        fputc(prev, outfile);
    }

    fclose(infile);
    fclose(outfile);

    return 0;
}
