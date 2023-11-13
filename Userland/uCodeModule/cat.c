// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <cat.h>

void cat(int argc, char *args[])
{
    char c[3] = {0};
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

        if (c[0] == 0 && c[1] == 0)
        {
            buff[i] = 0;
            printf("\n%s\n", buff);
            i = 0;
        }

    } while ((c[0] != 4 && c[0] == 0) || (c[0] != 4 && c[1] == 0));
    free(buff);
}