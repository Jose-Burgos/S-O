#include "scheduler.h"
#include <process.h>
#include <videoDriver.h>
#include <syscallManager.h>
#include <semaphore.h>
#include "include/lib.h"

nodeP root = NULL;
nodeP currentNode = NULL;
nodeP foreground = NULL;
nodeP background = NULL;

uint64_t forceNext = 0;
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
    nodeP first = malloc(sizeof(node));

    if(first==NULL) {
        printString((uint8_t *)"Memory Error", RED);
        return;
    }

    char *argv[] = {NULL};
    first->p = (process *)createProcess("IDLE", argv, &idle, QUANTUM_MAX);
    first->quantums = QUANTUM_MAX;
    root = first;
    first->next = root;
    currentNode = root;
    currentNode->p->state = RUNNING;
    nodeCount++;
    sem_open(SCHEDULER_SEM,1);
    sem_open(CHILDREN_SEM,1);
    sem_open(PID_SEM,1);

}

uint64_t schedule(uint64_t SP) {

    if(!pendingDisables) {

        if(nodeCount == 1) {
            return SP;
        }

        currentNode->p->SP = SP;

        if(currentNode->p->state == RUNNING) {
            currentNode->quantums++;
        }
        else if(currentNode->p->state == KILLED) {

            if(currentNode->p == foreground->p) {
                killFgroundProcess();
            } else {
                removeNode(currentNode);
            }
            runNext(currentNode);
            return currentNode->p->SP;
        }

        if(currentNode->p->state == BLOCKED) {
            runNext(currentNode->next);
        } 

        else if(currentNode->quantums > QUANTUM_MAX || forceNext) {
            forceNext = 0;
            currentNode->p->state = READY;
            runNext(currentNode->next);
        }
    }
    return currentNode->p->SP;
}

void disableScheduler() {
    int count;
    do {
        count = pendingDisables;
    } while (_cmpxchg(&pendingDisables, count + 1, count) != count);
}

void enableScheduler() {
    if (pendingDisables > 0) {
        pendingDisables--;
    }
}

void forceNextSwitch() {
    forceNext = 1;
    _forceScheduler();
}

static void runNext(nodeP from) {
    currentNode = findNext(from);
    currentNode->p->state = RUNNING;
    currentNode->quantums = currentNode->p->priority;
}

uint64_t addProcess(char *name, char **argv, void *entryPoint, uint64_t priority, uint64_t fg_flag) {
    disableScheduler();

    processP p = createProcess(name, argv, entryPoint, priority);
    
    sem_wait(CHILDREN_SEM);
    currentNode->p->children++;
    sem_post(CHILDREN_SEM);

    nodeP new = insertNode(p);

    if(fg_flag) {
        if(background == NULL) {
            background = foreground;
        }
        foreground = new;
    }
    
    nodeCount++;
    enableScheduler();
    return p->pid;
}

static nodeP insertNode(processP process) {
    nodeP new = malloc(sizeof(node));
    if(new == NULL)     
    {
        printString((uint8_t *)"Memory Error", RED);
        return 0;
    }
    new->p = process;
    new->quantums = process->priority;
    new->next = currentNode->next;
    currentNode->next = new;
    return new;
}

void killFgroundProcess() {
    if(background == NULL) {
        printf("No background process\n");
        return;
    }
    nodeP toKill = foreground;
    foreground = background;

    if(foreground->p->state == BLOCKED) {
        foreground->p->state = READY;
    }

    background = NULL;
    killProcess(toKill->p->pid);
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
    _forceScheduler();
    return;
}

void blockCurrentProcess() {
    currentNode->p->state = BLOCKED;
    _forceScheduler();
    return;
}

uint64_t killProcess(uint64_t pid) {

    if(currentNode->p->pid == pid) {
        killCurrentProcess();
    }

    nodeP aux = findNode(pid);
    if(aux == NULL) {
        printString((uint8_t *)"INEXISTENT PID\n", RED);
        return -1;
    }
    aux->p->state = KILLED;
    uint64_t pid_killed = aux->p->pid; 
    removeNode(aux);
    return pid_killed;
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
        n->p = n->next->p;
        n->next = n->next->next;
        if(n->p->pid == 0) {
            root = n;
        }
        free(toFree);
    }
}

static nodeP findNext(nodeP candidate) {
    if(candidate->p->state == READY || candidate->p->state == NEW) {
        return candidate;
    }
    return findNext(candidate->next);
}

uint64_t blockProcess(uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->state = BLOCKED;
        _forceScheduler();
        return aux->p->pid; 
    }
    return -1;
}

uint64_t processReady(uint64_t pid) {
    nodeP aux = findNode(pid);
    if(aux != NULL) {
        aux->p->state = READY;
        return aux->p->pid;
    }
    return -1;
}

static void printNode(nodeP n) {
    const char *stateStrings[] = {
        "READY",
        "RUNNING",
        "BLOCKED",
        "NEW",
        "KILLED"
    };
    printf("|\t%s\t|\t%d\t|\t%d\t|\t%s\t|\t%p\t|\t%p\t|\t%d\t|\n", n->p->name, n->p->pid, n->p->priority, stateStrings[n->p->state], n->p->stack, n->p->BP, n->p->pid == foreground->p->pid ? 1 : 0);
}

void printProcesses() { // TODO: print the list from the buttom to the top
    nodeP aux = root;
    printString((uint8_t *)"|\tProc:\t|\tID:\t|\tPrio:\t|\tState:\t|\tStack:\t|\tBP:\t|\tFG:\t|\n", GRAY);
    printNode(aux);
    aux = aux->next;
    while (aux != root) {
        printNode(aux);
        aux = aux->next;
    }
}

processP getCurrentProcess() {
    return currentNode->p;
}

uint64_t getCurrentPID() {
    return currentNode != NULL ? currentNode->p->pid : 0;
}