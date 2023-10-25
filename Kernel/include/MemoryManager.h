#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BUDDY
#define MIN_BLOCK_SIZE sizeof(MemBlock) + 100

typedef struct info_Mem {
    size_t allocated;
    size_t free;
    size_t total;
} info_Mem;

int initalizeMemoryManager(void *initialAddress, size_t size);
void *allocMemory(size_t size);
void getInfoMem(info_Mem *infoMemPtr);
void free(void *ptr);

#endif
