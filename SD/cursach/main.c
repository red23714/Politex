#include "lz77.h"
#include "lzw.h"

int main(int argc, char* argv[]) 
{
    if(argc < 3)
    {
        perror("Missing arguments");
        return 1;
    }

    FILE* input = fopen(argv[3], "rb");
    if (!input) 
    {
        perror("Error opening input file");
        return 1;
    }

    FILE* output = fopen(argv[4], "wb");
    if (!output) 
    {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    if(argv[2][0] == '-') 
    {
        switch (argv[2][1]) 
        {
            case 'c':
                if(strcmp(argv[1], "77")) lz77_compress(input, output);
                else if (strcmp(argv[1], "w")) lzw_compress(input, output);  
            break; 
            case 'd':
                if(strcmp(argv[1], "77")) lz77_decompress(input, output);
                else if (strcmp(argv[1], "w")) lzw_decompress(input, output);
            break; 
        }
    } 

    fclose(input);
    fclose(output);

    return 0;
}
