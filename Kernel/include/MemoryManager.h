#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdint.h>

#define MIN_BLOCK_SIZE sizeof(MemBlock) + 100

/* typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);

void *allocMemory(const uint64_t memoryToAllocate); */

typedef struct info_Mem {
    size_t allocated;
    size_t free;
    size_t total;
} info_Mem;

static info_Mem infoMem;

#endif
