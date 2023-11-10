#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdint.h>
#include <MemoryManager.h>

#define PID_MUTEX ""

#define STACK_SIZE 4000 //CHEQUEAR

typedef enum { READY = 0, RUNNING, BLOCKED, NEW, KILLED } states;

typedef struct process {
    char *name;
    char **argv;
    void *stack;
    states state;
    uint64_t pid;    
    uint64_t priority;
    uint64_t SP; //stack pointer
    uint64_t BP; //base pointer
    uint64_t fg; //foreground
    uint64_t children;
    uint64_t parent_pid;
} process;

typedef struct process * processP;

processP createProcess(char *name, char **argv, void *entryPoint, uint64_t priority, uint64_t fg_flag);

void freeProcess(processP p);

void wait_pid();

#endif