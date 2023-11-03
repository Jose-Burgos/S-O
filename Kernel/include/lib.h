#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *strcpy(char *dest, const char *src);
int strlen(char *str);

char *cpuVendor(char *result);
char keyPressed(void);

int strlength(char* str);

#endif
