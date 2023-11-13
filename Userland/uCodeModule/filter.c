// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <filter.h>

void filter(int argc, char **argv)
{
    char c[4] = {0, 0, 1, 1};
    char *buff = malloc(BUFF_MAX);
    if (buff == NULL)
        return;

    int i = 0;

    do
    {
        read(STDIN, (char *)&c, 2);

        if (c[0] != 0 && c[0] != 4)
        {
            putChar(c[0]);
            buff[i++] = c[0];
        }

        if (c[1] != 0 && c[1] != 4)
        {
            putChar(c[1]);
            buff[i++] = c[1];
        }

    } while ((c[0] != 4 && c[0] != 0) || (c[1] != 4 && c[1] != 0));

    buff[i] = 0;

    int j = 0;
    for (i = 0; buff[i] != 0; i++) {
        if (isVowel(buff[i]))
            buff[j++] = buff[i];
    }
    buff[j] = '\0';

    printf("\nFiltered vowels: %s\n", buff);
    free(buff);
}

int isVowel(char c)
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}