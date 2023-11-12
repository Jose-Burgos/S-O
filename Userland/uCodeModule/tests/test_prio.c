// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include "../include/lib.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 5  
#define MEDIUM 3  
#define HIGHEST 0 

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

static unsigned long fd[2] = {0, 1}; // READ, WRITE

void test_prio(){
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {NULL};
    uint64_t i;
    printf("\nCREATING PROCESSES...\n");
    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = exec("endless_loop_print", argv, &endless_loop_print, HIGHEST, 0, fd);
    shortSleep(225);
    bussy_wait(WAIT);
    printf("\nCHANGING PRIORITIES...\n");
    for (i = 0; i < TOTAL_PROCESSES; i++)
        nice(prio[i], pids[i]);
    shortSleep(225);
    bussy_wait(WAIT);
    printf("\nBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        block(pids[i]);
    shortSleep(225);
    printf("\nCHANGING PRIORITIES WHILE BLOCKED...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        nice(MEDIUM, pids[i]);
    shortSleep(225);
    printf("UNBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        ready(pids[i]);
    shortSleep(225);
    bussy_wait(WAIT);
    printf("\nKILLING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        kill(pids[i]);
}
