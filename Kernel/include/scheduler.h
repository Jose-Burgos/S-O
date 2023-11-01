#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <timer.h>
#include "process.h"
#include <stdint.h>

#define QUANTUM_MAX 8

typedef struct node {
    uint64_t quantums;
    processP p;
    struct node *next;
} node;

typedef node * nodeP;

void init_scheduler();

uint64_t addProcess(char *name, char **argv, void *entryPoint, uint64_t priority);

void changePriority(uint64_t priority, uint64_t pid);

void killCurrentProcess();