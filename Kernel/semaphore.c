#include "include/semaphore.h" 

sem_ts semaphores[MAX_SEMS];

static int semInit(char *name, int value);
static int getName(char *name);
static int findSem();
static char exist(int i);
static void updateProc(p_sem sem);
static char checkSem(p_sem sem);

int sem_open(char *name, int value) {
    int idx = getName(name);
    if (idx == -1) 
        if ((idx = semInit(name, value)) == -1)
            return -1;
    
    p_sem sem = &semaphores[idx];
    int amount;
    do
        amount = sem->open_count;
    while(_cmpxchg(&sem->open_count, amount + 1, amount) != amount);
    return 0;
}

int sem_wait(char *name) {
    int index = getName(name);
    if(index == -1) return -1;
    p_sem sem = &semaphores[index];
    while (!checkSem(sem)) {
        int pid = getCurrentPID();
        disableScheduler();
        uint64_t last;
        do {
            last = sem->blocked_last;
            sem->blocked_processes[last % MAX_PROC] = pid;
        } while (_cmpxchg(&sem->blocked_last, last + 1, last) != last);        
        if (checkSem(sem)) {
            while ((last = sem->blocked_last) - sem->blocked_first > 0 &&
                   _cmpxchg(&sem->blocked_last, last - 1, last) != last) ; 
            processReady(pid);
            enableScheduler();
            break;
        }
        enableScheduler();
        forceNextSwitch();
    }
    sem->value--;
    sem->lock = 0;
    return 0;
}

int sem_post(char *name) {
    int index = getName(name);
    if (index == - 1) return -1;
    p_sem sem = &semaphores[index];
    disableScheduler();
    uint64_t value;
    do
        value = sem->value;
    while (_cmpxchg(&sem->value, value + 1, value) != value);
    updateProc(sem);
    enableScheduler();
    return 0;
}

int sem_close(char *name) {
    int index = getName(name);
    if(index == -1) return -1;
    p_sem sem = &semaphores[index];
    uint64_t count = --(sem->open_count);
    if((int)count < 1) {
        sem->sem_id = 2 * index;
        strcpy(sem->name, "");
    }
    return 0;
}

int get_sem_count() {
    int count;
    for(int i = 0; i < MAX_SEMS; i++)
        if(exist(semaphores[i].sem_id)) count++;
    return count;
}

int get_sem_info(int index, p_sem buffer) {
    if (index < 0 || index >= MAX_SEMS) return -1;
    buffer->sem_id = semaphores[index].sem_id;
    strcpy(buffer->name, semaphores[index].name);
    buffer->value = semaphores[index].value;
    for (int j = 0; j < semaphores[index].open_count; j++) {
        buffer->blocked_processes[j] = semaphores[index].blocked_processes[j];
        buffer->open_count++;
    }
    return 0;
}

static int semInit(char *name, int value) {
    int index = findSem();
    if(index == -1) return -1;
    semaphores[index].sem_id = index * 2 + 1; 
    semaphores[index].open_count = 0;
    semaphores[index].value = value;
    semaphores[index].blocked_last = 0;
    semaphores[index].blocked_first = 0;
    semaphores[index].lock = 0;
    strcpy(semaphores[index].name , name);
    for(int i = 0; i < MAX_PROC; i++)
        semaphores[index].blocked_processes[i] = -1;
    return 0;
}

static char exist(int i) {
    if(i % 2 != 0)
        return 1;
    else
        return 0;
}

static int findSem() {
    for(int i = 0; i < MAX_SEMS; i++)
        if(!exist(semaphores[i].sem_id)) return i;
    return -1;
}

static int getName(char *name) {
    for(int i = 0; i < MAX_SEMS; i++)
        if(strcmp(semaphores[i].name, name) == 0 && exist(semaphores[i].sem_id))
            return i;
    return -1;
}

static void updateProc(p_sem sem) {
    if (sem->blocked_last == sem->blocked_first) {
        return;
    }
    uint64_t first_idx = sem->blocked_first;
    int pid = sem->blocked_processes[first_idx % MAX_PROC];
    if (pid != -1) {
        if (_cmpxchg(&sem->blocked_first, first_idx + 1, first_idx) == first_idx) {
            processReady(pid);
            sem->blocked_processes[first_idx % MAX_PROC] = -1;
        }
    }
}

static char checkSem(p_sem sem) {
    if (!_xchg(&sem->lock, 1) && (sem->value > 0 || (sem->lock = 0)))
        return 1;
    return 0;
}