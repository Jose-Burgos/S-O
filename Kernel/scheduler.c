#include "scheduler.h"
#include <process.h>
#include <videoDriver.h>
#include <syscallManager.h>

nodeP root;
nodeP currentNode;

void idle() {
    while(1) {
        sys_wait(10);
    }
}

void init_scheduler() {
    nodeP first = allocMemory(sizeof(node));
    if(first==NULL) {
       printString((uint8_t *)"MemError", RED);
        return;
    }
    char *argv[] = {NULL};
    first->p = creeateProcess("IDLE", argv, &idle, QUANTUM_MAX);
    first->quantums = QUANTUM_MAX;
    first->next = first;
    root = first;
    currentNode = first;
    currentNode->p->state = RUNNING;
}

uint64_t addProcess(char *name, char **argv, void *entryPoint, uint64_t priority) {
    processP p = createProcess(name, argv, entryPoint, priority>QUANTUM_MAX?QUANTUM_MAX:priority);
    nodeP new = insertNode(p);
}

static nodeP insertNode(processP process) {
    nodeP new = allocMemory(sizeof(node));
    if(new == NULL)     
    {
        printString((uint8_t *)"MemError", RED);
        return 0;
    }
    new->p = process;
    new->quantums = process->priority;
    new->next = currentNode->next;
    currentNode->next = new;
    return new;
}


static nodeP findNode(uint64_t pid) {
    if(root->p->pid == pid) {
        return root;
    }
    for(nodeP aux = root->next; aux!=root; aux = aux->next) {
        if(aux->p->pid == pid) {
            return aux;
        }
    }
    return NULL;
}

void changePriority(uint64_t priority, uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->priority = priority;
    }
}

void killCurrentProcess() {
    currentNode->p->state = KILLED;
}

void blockCurrentProcess() {
    currentNode->p->state = BLOCKED;
}

void processReady(uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->state = READY;
    }
}

void killProcess(uint64_t pid) {
    nodeP aux = findNode(pid);
    aux->p->state = KILLED;
    removeNode(aux);
}

static removeNode(nodeP n) {
    //TODO
}

static nodeP findNext(nodeP candidate) {
    if(candidate->p->state == READY || candidate->p->state == NEW) {
        return candidate;
    }
    return (findNext(candidate->next));
}

