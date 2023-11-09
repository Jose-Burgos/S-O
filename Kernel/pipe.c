#include "include/pipe.h"
#include <lib.h>
#include "include/lib.h"

pipe_t pipes[MAX_PIPES];

void freeProcPipe(int *processes, int currentPid);
int pipeWrite(int index, char *buff, int n);
int pipeRead(int index, char *buff, int n);
int pipeOpen(char* name);
void pipeClose(int index);
int findPipe(const char *name);
int nextAvailPipe();
void cleanData(char *data, int len);
void freeProcPipe(int *processes, int currentPid);
void blockProcPipe(int *processes, int currentPid);

int pipeRead(int index, char *buff, int n) {
    if(pipes[index].initialized == 0)
        return - 1;
    
    pipe_t pipe = pipes[index];
    //Me aseguro que solo haya un proceso para leer del pipe a la vez
    blockProcPipe(pipe.rPids, getCurrentPID());
    sem_wait(pipe.name);
    freeProcPipe(pipe.wPids, getCurrentPID());

    int i;
    //Verifico de no llegar al ultimo caracter del buffer para poder terminar en /0, a su vez no haber llegado al final la data (-1), y que la pos del read este por detras de escritura.
    for(i = 0; i < n - 1 && pipe.data[pipe.posread % PIPE_BUFFER_SIZE] != - 1 && (pipe.posread + 1) != (pipe.poswrite); i++) {
        buff[i] = pipe.data[pipe.posread % PIPE_BUFFER_SIZE];
        pipe.posread++;
    }
    //Si llego al final de la lectura, copio el ultimo y termino /0
    if(pipe.posread == pipe.poswrite - 1) {
        buff[i] = pipe.data[pipe.posread % PIPE_BUFFER_SIZE];
        buff[i + 1] = 0;
        pipe.posread++;
    }
    else
        buff[i] = 0;

    sem_post(pipe.name);
    return i;
}

int pipeWrite(int index, char *buff, int n) {
    if(pipes[index].initialized == 0)
        return - 1;
    
    pipe_t pipe = pipes[index];
    blockProcPipe(pipe.wPids, getCurrentPID());
    sem_wait(pipe.name);
    freeProcPipe(pipe.wPids, getCurrentPID());

    int i;
    for(i = 0; i < n && buff[i] != 0 && (pipe.poswrite) != (pipe.posread + 1); i++)
        pipe.data[pipe.poswrite++ % PIPE_BUFFER_SIZE] = buff[i];
    pipe.data[pipe.poswrite % PIPE_BUFFER_SIZE] = - 1; //Sentinel value

    sem_post(pipe.name);
    return i;
}

int pipeOpen(char* name) {
    int i;
    if( (i = findPipe(name))  != - 1)
        return pipes[i].fd;
    
    int index = nextAvailPipe();
    if(index == - 1)
        return - 1;

    pipe_t pipe = pipes[index];
    //Inicializacion de los campos del pipe 
    strcpy(pipe.name, name);
    pipe.fd = index + INITIAL_FD;
    pipe.initialized = 1;
    pipe.in_use = 0;
    pipe.posread = 0;
    pipe.poswrite = 0;
    cleanData(pipe.data, PIPE_BUFFER_SIZE);

    //Inicializacion del semaforo
    sem_open(name, 1);

    return pipes[index].fd;
}

void pipeClose(int index) {
    if(pipes[index].initialized != 1)
        return;
    pipe_t pipe = pipes[index];    

    sem_close(pipe.name);
    cleanData(pipe.data, PIPE_BUFFER_SIZE);
    cleanData(pipe.name, MAX_PIPE_NAME_LENGTH);
    
    for(int i = 0 ; i < MAX_PROCESSES_PER_PIPE; i++) {
        pipe.wPids[i] = - 1;
        pipe.rPids[i] = - 1;
    }

    pipe.posread = 0;
    pipe.poswrite = 0;
    pipe.initialized = 0;
    return;
}

int findPipe(const char *name) {
    for(int i = 0; i < MAX_PIPES; i++)
        if(strcmp(pipes[i].name, name) == 0)
            return i;
    return -1;
}

int nextAvailPipe() {
    for(int i = 0; i < MAX_PIPES; i++)
        if(pipes[i].in_use == 0)
            return i;
    return - 1;    
}


void cleanData(char *data, int len) {
    for(int i = 0; i < len; i ++)
        data[i] = 0;
    return;
}

void blockProcPipe(int *processes, int currentPid) {
    int index;
    for(index = 0; index < MAX_PROCESSES_PER_PIPE && processes[index] != 0; index ++) {
        //El proceso ya estaba en el vector de PIDS, solamente llamo a que se bloquee.
        if(processes[index] == currentPid) {
            blockProcess(currentPid);
            return;
        }
    }
    //No hay lugar para el proceso
    if(index < MAX_PROCESSES_PER_PIPE) {
        processes[index] = currentPid;
        blockProcess(currentPid);
    }
    return;
}

void freeProcPipe(int *processes, int currentPid) {
    for(int index = 0; index < MAX_PROCESSES_PER_PIPE; index++)
        if(processes[index] == currentPid) {
            processes[index] = - 1;
            processReady(currentPid);
            return;
        }
}

char *pipesInfo() {
    return pipes[0].name; // TODO: return the struct !?
}