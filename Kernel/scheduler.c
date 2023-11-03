#include "scheduler.h"
#include <process.h>
#include <videoDriver.h>
#include <syscallManager.h>
#include <semaphore.h>

nodeP root = NULL;
nodeP currentNode = NULL;

uint64_t forceChange = 0;
uint64_t pendingDisables = 0;
uint64_t nodeCount = 0;

static void runNext(nodeP from);
static nodeP findNext(nodeP candidate);
static nodeP insertNode(processP process);
static nodeP findNode(uint64_t pid);
static void removeNode(nodeP n);

void idle() {
    while(1) {
        sys_wait(10);
    }
}

void init_scheduler() {

    sem_open(SCHEDULER_SEM,1);
    sem_open(CHILDREN_SEM,1);
    sem_open(PID_MUTEX,1);

    nodeP first = malloc(sizeof(node));

    if(first==NULL) {
        printString((uint8_t *)"Memory Error", RED);
        return;
    }

    char *argv[] = {NULL};
    first->p = creeateProcess("IDLE", argv, &idle, QUANTUM_MAX);
    first->quantums = QUANTUM_MAX;
    first->p->state = RUNNING;
    first->next = first;
    currentNode = first;
    root = first;
    nodeCount++;
}

uint64_t schedule(uint64_t SP) {

    if(!pendingDisables) {

        if(nodeCount == 1) {
            return SP;
        }

        currentNode->p->SP = SP;

        if(currentNode->p->state == RUNNING) {
        currentNode->quantums--;
        }

        if(currentNode->p->state == KILLED) {
            removeNode(currentNode);
            runNext(currentNode);
            return currentNode->p->SP;
        }

        if(currentNode->p->state == BLOCKED) {
            runNext(currentNode->next);
        } 

        else if(currentNode->quantums == 0 || forceChange) {
            currentNode->p->state = READY;
            runNext(currentNode->next);
            forceChange = 0;
        }
    }
    return currentNode->p->SP;
}

void disableScheduler() {
    int count;
    while(1) {
        count = pendingDisables;
        if(_cmpxchg(&pendingDisables, count + 1, count) != count) {
            break;
        }
    }
}

static void runNext(nodeP from) {
    currentNode = findNext(from);
    currentNode->p->state = RUNNING;
    currentNode->quantums = currentNode->p->priority;
}

uint64_t addProcess(char *name, char **argv, void *entryPoint, uint64_t priority) {
    disableScheduler();

    processP p = createProcess(name, argv, entryPoint, priority>QUANTUM_MAX?QUANTUM_MAX:priority);
    
    sem_wait(CHILDREN_SEM);
    currentNode->p->children++;
    sem_post(CHILDREN_SEM);

    nodeP new = insertNode(p);
    nodeCount++;

    if(pendingDisables > 0) {
        pendingDisables--;
    }
}

static nodeP insertNode(processP process) {
    nodeP new = malloc(sizeof(node));
    if(new == NULL)     
    {
        printString((uint8_t *)"Memory Error", RED);
        return 0;
    }
    currentNode->next = new;
    new->next = currentNode->next;
    new->p = process;
    new->quantums = process->priority;
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
    forceScheduler();
}

void blockCurrentProcess() {
    currentNode->p->state = BLOCKED;
    forceScheduler();
}

void processReady(uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->state = READY;
    }
}

void killProcess(uint64_t pid) {

    if(currentNode->p->pid == pid) {
        killCurrentProcess();
    }

    nodeP aux = findNode(pid);
    if(aux == NULL) {
        printString((uint8_t *)"INEXISTENT PID\n", RED);
    }
    aux->p->state = KILLED;
    removeNode(aux);
}

static void removeNode(nodeP n) {
    nodeP parent = findNode(currentNode->p->parent_pid);
    if(parent != NULL) {

        sem_wait(CHILDREN_SEM);
        if(--parent->p->children == 0) {
            processReady(parent->p->pid);
        }
        sem_post(CHILDREN_SEM);
        nodeCount--;

        nodeP toFree = n->next;
        freeProcess(n->p);
        n->p = toFree->p;
        if(n->p->pid == 0) {
            root = n;
        }
        n->next = toFree->next;
        free(toFree);
    }
}

static nodeP findNext(nodeP candidate) {
    if(candidate->p->state == READY || candidate->p->state == NEW) {
        return candidate;
    }
    return findNext(candidate->next);
}

void blockCurrentProcess() {
    currentNode->p->state = BLOCKED;
    forceScheduler();
}

void blockProcess(uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->state = BLOCKED;
    }
    forceScheduler();
}

void processReady(uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->state = READY;
    }
}

static printNode(nodeP n) {
    const char *stateStrings[] = {
        "READY",
        "RUNNING",
        "BLOCKED",
        "NEW",
        "KILLED"
    };
    printf("Process: %s\t ID: %d\tState: %s\t\n", n->p->name, n->p->pid, stateStrings[n->p->state]);
    //TODO: IMPLEMENTAR PRINTF EN LIB.C
}

void printProcesses() {
    printNode(root);
    for(nodeP aux = root->next; aux != root; aux = aux->next) {
        printNode(aux);
    }
}

processP getCurrentProcess() {
    return currentNode->p;
}

uint64_t getCurrentPID() {
    return currentNode != NULL ? currentNode->p->pid : 0;
}