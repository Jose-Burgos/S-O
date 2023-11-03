#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stdarg.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *strcpy(char *dest, const char *src);
int strleng(char *str);

int strcmp(const char *s1, const char *s2);

char *cpuVendor(char *result);
char keyPressed(void);

int strlength(char* str);

void printf(const char *fmt, ...);

#endif
