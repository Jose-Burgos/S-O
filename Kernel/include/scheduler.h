#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <timer.h>
#include <process.h>
#include <stdint.h>

#define QUANTUM_MAX 8

extern uint64_t _xchg(uint64_t *dest, uint64_t value);
extern uint64_t _cmpxchg(uint64_t *dest, uint64_t value, uint64_t test);

typedef struct node {
    uint64_t quantums;
    processP p;
    struct node *next;
} node;

typedef node * nodeP;

void init_scheduler();
uint64_t schedule(uint64_t SP);
void disableScheduler();
uint64_t addProcess(char *name, char **argv, void *entryPoint, uint64_t priority, uint64_t fg_flag);
void changePriority(uint64_t priority, uint64_t pid);
void killCurrentProcess();
//kill a specific process
uint64_t killProcess(uint64_t pid);
void blockCurrentProcess();
uint64_t blockProcess(uint64_t pid);
uint64_t processReady(uint64_t pid);
void printProcesses();
processP getCurrentProcess();
uint64_t getCurrentPID();
void forceScheduler();
void killFgroundProcess();
void fgroundProcessReady();
uint64_t _xchg(uint64_t *dest, uint64_t value);
uint64_t _cmpxchg(uint64_t *dest, uint64_t value, uint64_t test);
void _forceScheduler(void);
void ready_foreground_proc();
void enableScheduler();

#endif