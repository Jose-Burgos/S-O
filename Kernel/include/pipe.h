#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>

#define PIPE_SIZE 1024

enum { READ, WRITE } end;

typedef struct {
  char buffer[PIPE_SIZE];
  char fd;
  int remaining; // number of bytes remaining in the pipe
  int pid; // process id of the process that opened the pipe
  char ends[2]; // READ or WRITE
} pipe_t;

int open_pipe(pipe_t* pipe);
int read_pipe(pipe_t* pipe, char* buf, int len);
int write_pipe(pipe_t* pipe, char* buf, int len);
int close_pipe(pipe_t *pipe, uint8_t idx); // TODO: check if this is correct

#endif