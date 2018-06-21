#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 128

int main(int argc, char *argv[])
{
    FILE **files = malloc(sizeof(FILE *) * argc);
    files[0] = stdin;
    for (int i = 1; i < argc; i++)
    {
        files[i] = fopen(argv[i], "rb");
        if (files[i] == NULL)
        {
            fprintf(stderr, "ERROR: Cannot open file %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    char data[N + 1];
    int n;

    while (1)
    {
        for (int i = 0; i < argc; i++)
        {
            while ((n = fread(data, sizeof(char), N, files[i])) > 0)
            {
                for (int j = 0; j < n; j++)
                {
                    putchar(data[j]);
                }
            }
            rewind(files[i]); // Seek cursor of file to the beginning
        }
    }

    for (int i = 1; i < argc; i++)
    {
        fclose(files[i]);
    }

    return EXIT_SUCCESS;
}
