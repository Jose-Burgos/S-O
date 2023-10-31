#include <lib.h>
//#include <process.h>
//#include <scheduler.h>
#include <stdbool.h>
#include <stdint.h>
#include <lib.h>

#define MAX_PROC 50
#define MAX_SEMS 256
#define NAME_CHAR_LIMIT 1024
#define OUT_OF_BOUNDS -1
#define ERROR -1
#define NO_PID -1

typedef struct
{
    int sem_id;
    char name[NAME_CHAR_LIMIT];
    uint64_t lock; // lock
    uint64_t value;
    uint64_t open_count; // Counter of pending 'close' signals
    uint64_t blocked_processes[MAX_PROC];
    uint64_t blocked_first;
    uint64_t blocked_last;
} sem_ts;

typedef sem_ts *p_sem;

int sem_init(char *name, int value);
int sem_open(char *name, int value);
int sem_wait(char *name);
int sem_post(char *name);
int sem_close(char *name);
int get_sem_count();
int get_sem_info(int i, p_sem buffer);

sem_ts semaphores[MAX_SEMS];

int get_sem_by_name(char *name);
int get_new_sem();
bool is_in_use(int i);
void update_sem_processes(p_sem sem);

int sem_init(char *name, int value) {
    int index = get_new_sem();
    if(index == - 1)
        return ERROR;
    
    //Inicializo el semaforo en el index
    semaphores[index].sem_id = index * 2 + 1; //ID impar para que detecte que esta en uso.
    semaphores[index].open_count = 0;
    semaphores[index].value = value;
    semaphores[index].blocked_last = 0;
    semaphores[index].blocked_first = 0;
    semaphores[index].lock = 0;

    strcpy(semaphores[index].name , name);
    for(int i = 0; i < MAX_PROC; i++)
        semaphores[index].blocked_processes[i] = NO_PID;

    return 0;
}

void update_sem_processes(p_sem sem) {
    // Salir temprano si no hay procesos bloqueados.
    if (sem->blocked_last == sem->blocked_first) {
        return;
    }

    // Despertar al primer proceso bloqueado.
    uint64_t first_idx = sem->blocked_first;
    int pid = sem->blocked_processes[first_idx % MAX_PROC];
    if (pid != NO_PID) { // Asegúrese de que haya un proceso para despertar.
        if (_cmpxchg(&sem->blocked_first, first_idx + 1, first_idx) == first_idx) {
            // Asegúrese de que el proceso todavía está esperando antes de despertarlo.
            ready_process(pid); // Desbloquear proceso
            sem->blocked_processes[first_idx % MAX_PROC] = NO_PID; // Limpiar la entrada
        }
    }
}

bool is_in_use(int i) {
    if(i % 2 == 0)
        return true;
    else
        return false;
}

int get_new_sem() {
    for(int i = 0; i < MAX_SEMS; i++)
        if(!is_in_use(semaphores[i].sem_id))
            return i;

    return - 1;
}

int get_sem_by_name(char *name) {
    for(int i = 0; i < MAX_SEMS; i++)
        if(strcmp(semaphores[i].name, name) == 0 && is_in_use(semaphores[i].sem_id))
            return i;
        
    return - 1;
}