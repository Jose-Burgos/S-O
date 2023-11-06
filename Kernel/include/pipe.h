#ifndef PIPE_H_
#define PIPE_H_

#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include <lib.h>
#include <semaphore.h>

#define MAX_PIPES 50
#define PIPE_BUFFER_SIZE 3000 // Previously PIPE_SIZE
#define MAX_PROCESSES_PER_PIPE 10 // Previously PROCS
#define MAX_PIPE_NAME_LENGTH 100 // Previously NAME_MAX
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

typedef ssize_t (*pipe_read_write_func)(int index, char *addr, size_t n);

int pipeRead(int index, char *buff, int n);
int pipeWrite(int index, char *addr, int n);
void pipeClose(int index);
int pipeOpen(const char *name);
char *pipesInfo();

// Additional function to initialize the pipe system if needed
void initialize_pipe_system(void);

#endif // PIPE_H_
