#ifndef _SYSCALLS_USERLAND_H_
#define _SYSCALLS_USERLAND_H_
#include <color.h>
#include <stdint.h>
#include <stddef.h>

#define STDOUT 0
#define STDERR 1
#define STDIN 0
#define MAX_PROC 50
#define NAME_CHAR_LIMIT 1024

typedef struct info_mem {
    uint8_t * type;
    size_t allocated;
    size_t free;
    size_t total;
} info_mem;

typedef struct{
    int sem_id;
    char name[NAME_CHAR_LIMIT];
    uint64_t lock; // lock
    uint64_t value;
    uint64_t open_count; // Counter of pending 'close' signals
    uint64_t blocked_processes[MAX_PROC];
    uint64_t blocked_first;
    uint64_t blocked_last;
} sem_ts;

typedef sem_ts *p_sem;

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
// --- Memory ---
void * sys_malloc(int size);
void sys_free(void * ptr);
void sys_memorystatus(info_mem * info);
// --- Processes ---
uint64_t sys_exec(char *name,  char **argv, void *entryPoint, uint64_t priority, uint64_t fg_flag);
uint64_t sys_kill_process(uint64_t pid);
uint64_t sys_block_process(uint64_t pid);
uint64_t sys_process_ready(uint64_t pid);
uint64_t sys_get_pid();
void sys_wait_pid();
void sys_yield();
void sys_print_processes();
void sys_nice(long priority, long pid);
// --- Semaphores ---
int sys_sem_init(char *name, int value);
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
