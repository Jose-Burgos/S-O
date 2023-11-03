#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//#define BUDDY

#ifdef BUDDY
#define TREE_HEIGHT 15
#define BLOCK_SIZE 512
#define BLOCK_LIMIT (1 << (TREE_HEIGHT - 1))
#define SIZE(level) ((1 << (level)) * BLOCK_SIZE)
#define IS_POW2(x) (((x) != 0) && (((x) & ((x)-1)) == 0))
typedef struct Block {
    size_t size;
    size_t level;
    struct Block *next;
    struct Block *prev;
} Block;

typedef struct BuddyInfo {
    Block blocks[BLOCK_LIMIT];
    Block *freeRow[TREE_HEIGHT];
    size_t lvl;
    size_t used;
    void* start;
} BuddyInfo;
#else 

#define MIN_BLOCK_SIZE sizeof(MemBlock) + 100
typedef struct MemBlock {
    size_t size;
    struct MemBlock *next;
    struct MemBlock *prev;
} MemBlock;

typedef struct MemoryManager {
    MemBlock *alloc_mem;
    MemBlock *free_mem;
} MemoryManager;


#endif

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
