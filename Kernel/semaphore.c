//#include <process.h>
//#include <scheduler.h>
#include <stdbool.h>
#include <stdint.h>
#include "include/semaphore.h"
#include "include/lib.h"
#include "scheduler.h"
#include "process.h"

#define MAX_PROC 50
#define MAX_SEMS 256
#define NAME_CHAR_LIMIT 1024
#define OUT_OF_BOUNDS -1
#define ERROR -1
#define NO_PID -1

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

//Retorna el indice del vector semaphores donde se encuentra alojado
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

    return index;
}

int sem_open(char *name, int value) {
    int i;
    if( (i = get_sem_by_name(name)) == -1 ) {
        if( (i = sem_init(name, value)) == - 1 )
            return ERROR;
    }

    p_sem sem = &semaphores[i];
    int cant;
    do
        cant = sem->open_count;
    while(_cmpxchg(&sem->open_count, cant + 1, cant) != cant);
    return 0;
}

int sem_wait(char *name) {

    #define cond (!_xchg(&sem->lock, 1) && (sem->value > 0 || (sem->lock = 0)))

    int index = get_sem_by_name(name);
    if(index == - 1)
        return ERROR;
    
    p_sem sem = &semaphores[index];
    
    // Se entra en un bucle mientras la condición del semáforo no se cumple.
    while (!cond)
    {
        // Obtener el PID del proceso en ejecución.
        int pid = getCurrentPID();
        
        // Deshabilita el planificador para evitar condiciones de carrera.
        disableScheduler();
        
        uint64_t last;
        do
        {
            // Busca un lugar en la lista de procesos bloqueados y se añade a sí mismo.
            last = sem->blocked_last;
            sem->blocked_processes[last % MAX_PROC] = pid;
        }
        // Se realiza de manera atómica para asegurarse de que el proceso se añade correctamente.
        while (_cmpxchg(&sem->blocked_last, last + 1, last) != last);
        
        // Verifica si la condición cambió mientras se agregaba a la lista de bloqueados.
        if (cond)
        {
            // Si la condición cambió (probablemente por una interrupción que liberó el semáforo),
            // se deshace la operación de bloqueo.
            while ((last = sem->blocked_last) - sem->blocked_first > 0 &&
                   _cmpxchg(&sem->blocked_last, last - 1, last) != last)
                ; // Se realiza una disminución atómica de 'blocked_last'.
            
            processReady(pid); // Desbloquea el proceso.
            // scheduler_enable(); // Vuelve a habilitar el planificador.
            break; // Sale del bucle ya que ahora el semáforo está disponible.
        }
        
        // Si la condición no cambió, se habilita el planificador y se fuerza un cambio de contexto.
        // scheduler_enable();
        forceScheduler();
    }
    // Decrementa el valor del semáforo ya que el proceso actual pasará a tener el recurso.
    sem->value--;
    // Libera el bloqueo del semáforo.
    sem->lock = 0;

    return 0;
    #undef cond
}

int sem_post(char *name) {
    int index = get_sem_by_name(name);
    if (index == - 1)
        return ERROR;

    p_sem sem = &semaphores[index];
    
    disableScheduler();
    uint64_t value;
    
    do
        value = sem->value;
    while (_cmpxchg(&sem->value, value + 1, value) != value);
   
    update_sem_processes(sem);
    //scheduler_enable();
    
    return 0;
}

int sem_close(char *name) {
    int index = get_sem_by_name(name);
    if(index == - 1)
        return ERROR;

    p_sem sem = &semaphores[index];
    uint64_t count = --(sem->open_count);
    //Si open count es 0 podemos liberar el semaforo.
    if((int)count < 0) {
        sem->sem_id = 2 * index;
        strcpy(sem->name, "VACANT");
    }

    return 0;
}

int get_sem_count() {
    int count;
    for(int i = 0; i < MAX_SEMS; i++)
        if(is_in_use(semaphores[i].sem_id))
            count++;
    return count;
}

int get_sem_info(int index, p_sem buffer) {
    // Verifica si el índice es válido.
    if (index < 0 || index >= MAX_SEMS) {
        return ERROR;
    }

    // Asegúrate de que el semáforo esté en uso antes de copiar su información.
    if (!is_in_use(semaphores[index].sem_id)) {
        return ERROR;
    }

    // Copia la información básica del semáforo al buffer proporcionado por el usuario.
    *buffer = semaphores[index]; // Esto copiará todos los campos de una vez, asumiendo que es seguro hacerlo.

    return 0;
}


bool is_in_use(int i) {
    if(i % 2 != 0)
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

//void update_sem_processes(p_sem sem)
//{
//    uint64_t index;
//    bool awake = true;
//    while (sem->blocked_last - (index = sem->blocked_first) > 0 || (awake = false)) {
//        if (_cmpxchg(&sem->blocked_first, index + 1, index) == index)
//            break;
//    }
//    if (awake) {
//        int pid = sem->blocked_processes[index % MAX_PROC];
//        ready_process(pid);                               
//    }
//}

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
            processReady(pid); // Desbloquear proceso
            sem->blocked_processes[first_idx % MAX_PROC] = NO_PID; // Limpiar la entrada
        }
    }
}