#include <pipe.h>
#include <lib.h>

int open_pipe(pipe_t* pipe) {
    for (int i = 0; i < PIPE_SIZE; i++) {
    pipe->buffer[i] = 0;
    }
    pipe->pid = -1;
    pipe->remaining = PIPE_SIZE;
    pipe->ends[READ] = 1;
    pipe->ends[WRITE] = 1;
    pipe->fd = 1 + 3; // 1 is stdin, 2 is stdout, 3 is stderr
    return pipe->fd;
}

int read_pipe(pipe_t* pipe, char* buf, int len) {
    if (pipe->ends[READ] == 0) {
        return -1;
    }    
    if (pipe->remaining == PIPE_SIZE) {
        return -1;
    }
    int i;
    for (i = 0; i < len; i++) {
        buf[i] = pipe->buffer[i % PIPE_SIZE];
        pipe->remaining--;
    }
    return i;
}

int write_pipe(pipe_t* pipe, char* buf, int len) {
    if (pipe->ends[WRITE] == 0) {
        return -1;
    }
    if (len == 0) {
        len = strlength(buf);
    }
    if (pipe->remaining > len) {
        return -1;
    }
    
    int i;
    for (i = 0; i < len; i++) {
      pipe->buffer[i % PIPE_SIZE] = buf[i];
      pipe->remaining++;
    }
    return i;
}

int close_pipe(pipe_t *pipe, uint8_t idx) { // TODO: check if this is correct
    switch (idx) {
    case READ:
        pipe->ends[idx] = 0;
        return 0;
    case WRITE:
        pipe->ends[idx] = 0;
        return 0;
    
    default:
        return -1;
    }    
}
