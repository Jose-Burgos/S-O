#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdint.h>
#include "MemoryManager.h"

#define PID_MUTEX ""

#define STACK_SIZE 4000

typedef enum { READY = 0, RUNNING, BLOCKED, NEW, KILLED } states;

typedef struct process {
    char *name;
    char **argv;
    void *stack;
    states state;
    uint64_t pid;    
    uint64_t priority;
} process;

typedef struct process * processP;

processP createProcess(char *name, char **argv, void *entryPoint, uint64_t priority);

void freeProcess(processP p);

void wait_pid();

#endif