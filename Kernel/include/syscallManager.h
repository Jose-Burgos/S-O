#ifndef _SYSCALL_MANAGER_H_
#define _SYSCALL_MANAGER_H_

#include <timer.h>
#include <stdint.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <interrupts.h>
#include <MemoryManager.h>

#define STDIN 0
#define STDERR 1
#define STDOUT 0


#define REGISTER_NUM 17
#define REGISTER_NAMES {"RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"}


uint64_t sys_write(uint8_t fd, char *string, Color color);
uint64_t sys_read(uint8_t fd, char * buf, uint32_t count);
uint64_t sys_writeAt(uint16_t x, uint16_t y, char *string, Color color);
uint64_t sys_time();
uint64_t sys_date();
uint64_t sys_clearScreen();
uint64_t sys_drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);
uint64_t sys_beep(uint32_t frequency);
uint64_t sys_widthScr();
uint64_t sys_heightScr();
uint64_t sys_wait(uint32_t millis);
uint64_t sys_timedRead(uint8_t fd, char * buf, uint32_t count, uint32_t millis);
uint64_t sys_inforeg(uint64_t array[REGISTER_NUM]);
uint64_t sys_changeFontSize(uint32_t dif);
void * sys_malloc(uint64_t memSize);

#endif
