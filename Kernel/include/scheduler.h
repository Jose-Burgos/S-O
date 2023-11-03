#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <timer.h>
#include <process.h>
#include <stdint.h>

#define QUANTUM_MAX 8

typedef struct node {
    uint64_t quantums;
    processP p;
    struct node *next;
} node;

typedef node * nodeP;

void init_scheduler();

uint64_t schedule(uint64_t SP);

void disableScheduler();

uint64_t addProcess(char *name, char **argv, void *entryPoint, uint64_t priority);

void changePriority(uint64_t priority, uint64_t pid);

void killCurrentProcess();

//kill a specific process
void killProcess(uint64_t pid);

void blockCurrentProcess();

void processReady();

void printProcesses();

processP getCurrentProcess();

uint64_t getCurrentPID();

void forceScheduler();

uint64_t _xchg(uint64_t *dest, uint64_t value);

uint64_t _cmpxchg(uint64_t *dest, uint64_t value, uint64_t test);

#endif