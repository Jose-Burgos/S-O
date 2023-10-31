#include "process.h"
#include "lib.h"
#include "stack.h"

uint64_t current_pid = 0;

uint64_t count_args(char **argv);
char **copy_args(int argc, char **argv);
void free_args(char **argv);

processP createProcess(char *name, char **argv, void *entryPoint) {
    processP p = allocMemory(sizeof(process));
    if(p == NULL) {
        return NULL;
    }
    void *stack = allocMemory(STACK_SIZE);
    if(stack == NULL) {
        free(p);
        return NULL;
    }
    p->name = name;
    p->state = NEW;
    int argc = count_argv(argv);
    p->argv = copy_args(argc,argv);
    char *stackBase = (char *)stack + STACK_SIZE - sizeof(uint64_t);
    StackFrame sf = createStack(entryPoint, stackBase, argc, p->argv);
    memcpy((char *)stack + STACK_SIZE - sizeof(StackFrame), &sf, sizeof(StackFrame));
    p->stack = stack;
    p->pid = current_pid;
    current_pid++;
    return p;
}

void freeProcess(processP p) {
    free(p->stack);
    for(int i=0; p->argv[i]!=NULL; i++) {
        free(p->argv[i++]);
    }
    free(p->argv);
    free(p);
}

void wait_pid() {
    //
}


uint64_t count_argv(char **argv) {
    uint64_t toReturn = 0;
    while(argv[toReturn] != NULL) {
        toReturn++;
    }
    return toReturn;
}

char **copy_args(int argc, char **argv) {
    char **toReturn = allocMemory((argc + 1)*sizeof(char *));
    if(toReturn= NULL) {
        return NULL;
    }
    for(int i=0; i<argc; i++) {
        toReturn[i] = allocMemory(strlen(argv[i]) + 1);
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