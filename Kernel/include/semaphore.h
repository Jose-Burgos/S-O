#include <lib.h>
//#include <process.h>
//#include <scheduler.h>
#include <stdint.h>
#include <lib.h>

#define MAX_PROC 50
#define MAX_SEMS 256
#define NAME_CHAR_LIMIT 1024
#define OUT_OF_BOUNDS -1
#define ERROR -1
#define NO_PID -1

#define SCHEDULER_SEM "SCHEDULER_MUTEX"
#define CHILDREN_SEM "CHILDREN_MUTEX"
#define PID_SEM "PID_MUTEX"

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

extern uint64_t _xchg(uint64_t *dest, uint64_t value);
extern uint64_t _cmpxchg(uint64_t *dest, uint64_t value, uint64_t test);

int sem_open(char *name, int value);
int sem_wait(char *name);
int sem_post(char *name);
int sem_close(char *name);
int get_sem_count();
int get_sem_info(int idx, p_sem buffer);

