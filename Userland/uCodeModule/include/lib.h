#ifndef _LIB_H_
#define _LIB_H_
#include <stdint.h>
#include <color.h>
#include "syscalls.h"

#define true 1
#define false 0

int strcmp(const char *str1, const char *str2);
int strncmp (const char *str1, const char *str2, int n);
int strlen(const char *str1);
char * strcat(char *dest, const char *src);
long itoa(long number, char *str);
int getChar(void);
void printf(const char *fmt, ...);
int putColorChar(int car, Color c);
int putChar(int c);
void putStringColor(char *str, Color c);
void puts(char *fmt);
void scanf(int length, const char *fmt, ...);
void printStringAt(int x, int y, char *str, Color c);
void printString(char *str, Color c);
void printError(char * str);
void clear();
int getTimedChar();
long getScreenWidth();
long getScreenHeight();
long drawRectangle(int x, int y, int width, int height, Color color);
void shortSleep(int ticks);
void printBase(uint64_t value, uint32_t base);
void beep(int frequency);
void * malloc(unsigned int memSize);
void free(void * ptr);
void memStatus();
void exec(char *name,  char **argv, void *entryPoint, uint64_t priority, uint64_t fg_flag);
void kill(uint64_t pid);
void block(uint64_t pid);
void ready(uint64_t pid);
uint64_t getpid();
void waitpid();
void yield();
int sem_init(char *name, int value);
int sem_open(char *name, int value);
int sem_wait(char *name);
int sem_post(char *name);
int sem_close(char *name);
int sem_info(int i, p_sem buffer);
int sem_count();
int pipeRead(int index, char *buff, int n);
int pipeWrite(int index, char *addr, int n);
void pipeClose(int index);
int pipeOpen(char *name);
void pipesInfo();

#endif
