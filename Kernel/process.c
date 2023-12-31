// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "process.h"
#include "lib.h"
#include "stack.h"
#include <scheduler.h>
#include "include/semaphore.h"

uint64_t current_pid = 0;

uint64_t count_args(char **argv);
char **copy_args(int argc, char **argv);
void free_args(char **argv);

processP createProcess(char *name, char **argv, void *entryPoint, uint64_t priority, uint64_t fd[2]) {
    processP p = malloc(sizeof(process));
    if(p == NULL) {
        return NULL;
    }
    void *stack = malloc(STACK_SIZE);
    if(stack == NULL) {
        free(p);
        return NULL;
    }
    p->name = name;
    p->state = NEW;
    int argc = count_args(argv);
    p->argv = copy_args(argc,argv);

    char *stackBase = (char *)stack + STACK_SIZE - sizeof(uint64_t);
    stackFrame sf = createStack(entryPoint, stackBase, argc, p->argv);
    memcpy((char *)stack + STACK_SIZE - sizeof(stackFrame), &sf, sizeof(stackFrame));

    p->stack = stack;
    p->children = 0;
    p->priority = priority;
    p->fd[READ] = fd[READ];
    p->fd[WRITE] = fd[WRITE];

    sem_wait(PID_SEM);
    p->pid = current_pid++;
    sem_post(PID_SEM);
    p->parent_pid = getCurrentPID();
    p->SP = (uint64_t)stack + STACK_SIZE - sizeof(stackFrame);
    p->BP = p->SP;
    return p;
}

void freeProcess(processP p) {
    free(p->stack);
    for(int i=0; p->argv[i]!=NULL; i++) {
        free(p->argv[i++]);
    }
    current_pid--;
    free(p->argv);
    free(p);
}

void wait_pid() {
    if(getCurrentProcess()->children > 0) {
        blockCurrentProcess();
    }
}


uint64_t count_args(char **argv) {
    uint64_t toReturn = 0;
    while(argv[toReturn] != NULL) {
        toReturn++;
    }
    return toReturn;
}

char **copy_args(int argc, char **argv) {
    char **toReturn = malloc((argc + 1)*sizeof(char *));
    if(toReturn == NULL) {
        return NULL;
    }
    for(int i=0; i<argc; i++) {
        toReturn[i] = malloc(strleng(argv[i]) + 1);
        if(toReturn[i] == NULL) {
            for(int j=0; j<i; j++) {
                free(toReturn[j]);
            }
            free(toReturn);
        }
        strcpy(toReturn[i], argv[i]);
    }
    toReturn[argc] = NULL;
    return toReturn;
}