// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <wc.h>

int wc(int argc, char **argv)
{
    char c[3] = {0, 0, 0};
    char *buff = malloc(BUFF_MAX);
    if (buff == NULL) {
        return -1;
    }

    for (int i = 0; i < BUFF_MAX; i++) {
        buff[i] = 0;
    }

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
            putChar('\n');
            buff[i] = 0;
            i = 0;
        }

    } while ((c[0] != 4 && c[0] == 0) || (c[0] != 4 && c[1] == 0));

    long charCount = 0;
    long wordCount = 0;
    long lineCount = 0;
    int inWord = false;

    for (i = 0; buff[i] != '\0'; i++)
    {
        charCount++;
        if (buff[i] == 0) {
            lineCount++;
        }

        if (buff[i] == ' ' || buff[i] == '\t' || buff[i] == 0) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            wordCount++;
        }
    }

    // Increment line count if the last line didn't end with a newline
    if (buff[charCount - 1] == 0) {
        lineCount++;
    }

    printf("\n %d    %d    %d\n", charCount, wordCount, lineCount);

    free(buff);
    return 0;
}