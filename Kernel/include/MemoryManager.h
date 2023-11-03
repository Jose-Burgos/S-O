#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct info_Mem {
    size_t allocated;
    size_t free;
    size_t total;
} info_Mem;

int initalizeMemoryManager(void *initialAddress, size_t size);
void *malloc(size_t size);
void getInfoMem(info_Mem *infoMemPtr);
void free(void *ptr);

#endif
