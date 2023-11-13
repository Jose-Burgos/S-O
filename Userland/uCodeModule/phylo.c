#include <phylo.h>
#include <lib.h>

#define MAX 10
#define MIN 2
#define INITIAL 5

#define LEFT (((id) + (shared_data->count) - 1) % (shared_data->count))
#define RIGHT (((id) + 1) % (shared_data->count))

#define MUTEX "phylo/main"
#define SEM_PHYLO_H "phylo/t/0"
#define SEM_PHYLO_T "phylo/h/0"

#define SEM_LEN 10

typedef enum State
{
    THINKING,
    HUNGRY,
    EATING,
    LEAVING
} State;

typedef struct SharedData
{
    int count;
    State state[MAX];
    char hungry_lock[MAX][SEM_LEN];
    char think_unlock[MAX][SEM_LEN];
} SharedData;

static unsigned long fd[2] = {STDIN,STDOUT};

void test(SharedData *shared_data, int id)
{
    if (shared_data->state[id] == HUNGRY && shared_data->state[LEFT] != EATING &&
        shared_data->state[RIGHT] != EATING)
    {
        shared_data->state[id] = EATING;
        sem_post(shared_data->hungry_lock[id]);
    }
}

void take_fork(SharedData *shared_data, int id)
{
    sem_wait(MUTEX);
    shared_data->state[id] = HUNGRY;
    test(shared_data, id);
    sem_post(MUTEX);

    sem_wait(shared_data->hungry_lock[id]);
}

void put_fork(SharedData *shared_data, int id)
{
    sem_wait(MUTEX);
    shared_data->state[id] = THINKING;

    test(shared_data, LEFT);
    test(shared_data, RIGHT);

    sem_post(MUTEX);
}

int phylo_proc(int argc, char const *argv[])
{
    int id = atoul(argv[0]);
    SharedData *shared_data = (SharedData *)atoul(argv[1]);

    while (1)
    {
        take_fork(shared_data, id);
        yield();
        put_fork(shared_data, id);

        sem_post(shared_data->think_unlock[id]);
        yield();
        sem_wait(shared_data->think_unlock[id]);

        sem_wait(MUTEX);
        if (shared_data->state[id] == LEAVING)
        {
            sem_post(MUTEX);
            return 0;
        }
        sem_post(MUTEX);
    }
}

void addPhylo(SharedData *shared_data)
{
    int id = shared_data->count;
    char id_str[] = "X";
    char addr[64];
    char *argv_proc[3] = {id_str, addr, NULL};
    id_str[0] = '0' + id;
    ultoa((unsigned long)shared_data, addr);

    sem_wait(MUTEX);
    shared_data->state[id] = THINKING;
    shared_data->count++;
    exec("phylo_proc", argv_proc, &phylo_proc, 1, 0, fd);
    sem_post(MUTEX);
}

void removePhylo(SharedData *shared_data)
{
    size_t id = shared_data->count - 1;

    sem_wait(shared_data->think_unlock[id]);
    sem_wait(MUTEX);
    shared_data->state[id] = LEAVING;
    shared_data->count--;
    sem_post(MUTEX);
    sem_post(shared_data->think_unlock[id]);
}

int phylo(int argc, char *argv[])
{
    SharedData shared_data;
    shared_data.count = INITIAL;
    sem_open(MUTEX, 0);

    size_t i;
    for (i = 0; i < MAX; i++)
    {
        strcpy(shared_data.hungry_lock[i], SEM_PHYLO_H);
        strcpy(shared_data.think_unlock[i], SEM_PHYLO_T);

        shared_data.hungry_lock[i][SEM_LEN - 2] = '0' + i;
        shared_data.think_unlock[i][SEM_LEN - 2] = '0' + i;

        sem_open(shared_data.hungry_lock[i], 0);
        sem_open(shared_data.think_unlock[i], 0);
    }
    char id[] = "X";
    char addr[64];
    char *argv_proc[3] = {id, addr, NULL};

    for (i = 0; i < shared_data.count; i++)
    {
        id[0] = '0' + i;
        ultoa((unsigned long)&shared_data, addr);
        exec("phylo_proc", argv_proc, &phylo_proc, 1, 0, fd);
    }

    sem_post(MUTEX);

    while (1)
    {
        int c = getTimedChar();
        if (c == 'a' && shared_data.count < MAX)
        {
            addPhylo(&shared_data);
        }
        else if (c == 'r' && shared_data.count > MIN)
        {
            removePhylo(&shared_data);
        }

        sem_wait(MUTEX);
        char state[MAX + 1];
        int i;
        for (i = 0; i < shared_data.count; i++)
            state[i] = shared_data.state[i] == EATING ? 'E' : '.';

        state[i] = '\0';
        printf("%s\n", state);
        sem_post(MUTEX);
        yield();
    }
}