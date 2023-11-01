#ifndef _SYSCALLS_USERLAND_H_
#define _SYSCALLS_USERLAND_H_
#include <color.h>

#define STDOUT 0
#define STDERR 1
#define STDIN 0

long sys_read(unsigned char fd, char * s, int count);
long sys_write(unsigned char fd, char * s, Color c);
long sys_writeAt(short x, short y, char * s, Color c);
long sys_clearScreen();
long sys_wait(int ticks);
long sys_time();
long sys_date();
long sys_beep();
long sys_getScreenHeight();
long sys_getScreenWidth();
long sys_timedRead(unsigned char fd, char * s, int count, int millis);
long sys_drawRectangle(int x, int y, int width, int height, Color color);
long sys_inforeg(long * registers);
long sys_changeFontSize(int diff);
void * sys_malloc(int size);
void sys_free(void * ptr);

#endif
