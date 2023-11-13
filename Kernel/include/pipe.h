#ifndef _PIPE_H_
#define _PIPE_H_

#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include "lib.h"
#include <semaphore.h>

#define MAX_PIPES 50
#define PIPE_BUFFER_SIZE 3000 
#define MAX_PROCESSES_PER_PIPE 10 
#define MAX_PIPE_NAME_LENGTH 100 
#define INITIAL_FD 3

typedef struct {
    char name[MAX_PIPE_NAME_LENGTH];
    int initialized;
    int in_use;
    int fd;
    char data[PIPE_BUFFER_SIZE];
    int rPids[MAX_PROCESSES_PER_PIPE];
    int wPids[MAX_PROCESSES_PER_PIPE];
    int posread;
    int poswrite;
} pipe_t;

int pipeRead(int index, char *buff, int n);
int pipeWrite(int index, char *addr, int n);
void pipeClose(int index);
int pipeOpen(char *name);
char *pipesInfo();
void initialize_pipe_system(void);

#endif // PIPE_H_
