#ifndef _SYSCALL_MANAGER_H_
#define _SYSCALL_MANAGER_H_

#include <timer.h>
#include <stdint.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <interrupts.h>
#include <MemoryManager.h>
#include <scheduler.h>
#include <process.h>
#include "semaphore.h"
#include "pipe.h"
#include "lib.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2


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
// --- Memory ---
void * sys_malloc(uint64_t memSize);
void sys_free(void * ptr);
void sys_memory_status(info_Mem * info);
// --- Processes ---
uint64_t sys_exec(char *name,  char **argv, void *entryPoint, uint64_t priority, uint64_t fg_flag, uint64_t fd[2]);
uint64_t sys_kill_process(uint64_t pid);
uint64_t sys_block_process(uint64_t pid);
uint64_t sys_process_ready(uint64_t pid);
uint64_t sys_get_pid();
void sys_wait_pid();
void sys_yield();
void sys_print_processes();
void sys_nice(uint64_t priority, uint64_t pid);
// --- Semaphores --- 
int sys_sem_open(char *name, int value);
int sys_sem_wait(char *name);
int sys_sem_post(char *name);
int sys_sem_close(char *name);
int sys_sem_info(int i, p_sem buffer);
int sys_sem_count();
// --- Pipes ---
int sys_pipeRead(int index, char *buff, int n);
int sys_pipeWrite(int index, char *addr, int n);
void sys_pipeClose(int index);
int sys_pipeOpen(char *name);
char *sys_pipesInfo();


#endif
