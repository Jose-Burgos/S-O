#include <MemoryManager.h>
#include <videoDriver.h>

static info_Mem infoMem;

#ifdef BUDDY

#define TREE_HEIGHT 15
#define BLOCK_SIZE 512
#define BLOCK_LIMIT (1 << (TREE_HEIGHT - 1)) // Maximum number of blocks at level zero == 2^14
#define BLOCK_SIZE_LVL(level) ((1 << (level)) * BLOCK_SIZE)
#define IS_POWER_OF_TWO(x) ((x > 0) && ((x & (x - 1)) == 0))

/********************************************************************************************
                                        ERROR HANDLING
*********************************************************************************************/

#define AD_ERROR 1
#define SIZE_ERROR 2
#define MEM_FULL 3
#define OFF_LIMITS 4
#define ERROR -1

typedef struct Block {
    bool allocated;
    size_t height;
    struct Block *next;
    struct Block *prev;
} Block;

typedef struct Buddy {
    Block blocks[BLOCK_LIMIT]; // 2^14 blocks
    Block *freeBlocks[TREE_HEIGHT]; // List of free blocks by level
    size_t lvls;
    size_t ablocks;
} Buddy;

static Buddy *buddyInfo;

size_t log_2(size_t x);
Block *getNeighbour(Block *block_ptr);
void newBlock(Block *block_ptr);
void removeBlock(Block *block_ptr);
int splitBlock(size_t level);
void mergeBuddy(Block *current);

int initalizeMemoryManager(void *address, size_t size) {
    if ((size_t) address % sizeof(size_t) != 0)
        return AD_ERROR;

    if (size % BLOCK_SIZE != 0 || !IS_POWER_OF_TWO(size / BLOCK_SIZE))
        return SIZE_ERROR;

    size_t level = log_2(size / BLOCK_SIZE); // min level in the tree

    if (level == 0)
        return MEM_FULL;

    if (level >= TREE_HEIGHT)
        return OFF_LIMITS;

    buddyInfo = address;

    infoMem.allocated = 0;
    infoMem.free = size;
    infoMem.total = size;

    for (size_t i = 0; i < BLOCK_LIMIT; i++) {
        buddyInfo->blocks[i].allocated = false;
        buddyInfo->blocks[i].height = 0;
        buddyInfo->blocks[i].next = NULL;
        buddyInfo->blocks[i].prev = NULL;
    }

    for (size_t i = 0; i < TREE_HEIGHT; i++) {
        buddyInfo->freeBlocks[i] = NULL;
    }
        
    buddyInfo->blocks[0].height = level;
    buddyInfo->freeBlocks[level] = buddyInfo->blocks;
    buddyInfo->lvls = level + 1;
    buddyInfo->ablocks = 1 << level;


    return 0;
}

void *malloc(size_t size) {
    printString((uint8_t *)"Buddy Malloc\n", WHITE);
    size_t level = log_2(size / BLOCK_SIZE);
    if ((1 << level) * BLOCK_SIZE < size) {
        level++;
    }

    // If there is no block of the right size, search for a bigger one and split it the necessary number of times
    if (buddyInfo->freeBlocks[level] == NULL) {
        size_t min_above = level + 1;
        while (min_above < buddyInfo->ablocks && buddyInfo->freeBlocks[min_above] == NULL) {
            min_above++;
        }

        // No block found
        if (min_above == buddyInfo->lvls) {
            return NULL;
        }

        while (min_above > level) {
            splitBlock(min_above--);
        }
    }

    // Take block and return its memory address
    Block *aux_block = buddyInfo->freeBlocks[level];
    removeBlock(aux_block);
    return (void *)buddyInfo + (aux_block - buddyInfo->blocks) * BLOCK_SIZE;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }
    size_t offset = ptr - (void *)buddyInfo;
    if (offset % BLOCK_SIZE)
        return;

    Block *block_ptr = buddyInfo->blocks + (offset / BLOCK_SIZE);
    if (block_ptr >= buddyInfo->blocks + buddyInfo->ablocks || !block_ptr->allocated)
        return;

    newBlock(block_ptr);
    mergeBuddy(block_ptr);
}

size_t log_2(size_t x) {
    size_t ans = -1;
    while (x > 0) {
        x /= 2;   
        ans++;
    }
    return ans;
}

Block *getNeighbour(Block *current) {
    size_t l_idx = (current - buddyInfo->blocks) >> current->height;
    if (l_idx % 2) {
        return current - (1 << current->height);
    } else {
        return current + (1 << current->height);
    }
}

void newBlock(Block *current) {
    current->next = buddyInfo->freeBlocks[current->height];
    current->prev = NULL;

    buddyInfo->freeBlocks[current->height] = current;

    if (current->next != NULL)
        current->next->prev = current;

    current->allocated = false;
    infoMem.allocated -= BLOCK_SIZE_LVL(current->height);
    infoMem.free += BLOCK_SIZE_LVL(current->height);
}

void removeBlock(Block *current) {
    if (current->next) {
        current->next->prev = current->prev;
    }

    if (current->prev != NULL) {
        current->prev->next = current->next;
    }
    else {
        buddyInfo->freeBlocks[current->height] = current->next;
    }

    current->next = NULL;
    current->prev = NULL;
    current->allocated = true;
    infoMem.allocated += BLOCK_SIZE_LVL(current->height);
    infoMem.free -= BLOCK_SIZE_LVL(current->height);
}

int splitBlock(size_t level) {
    if (level == 0 || buddyInfo->freeBlocks[level] == NULL) {
        return ERROR;
    }

    Block *freeList = buddyInfo->freeBlocks[level];
    removeBlock(freeList);
    freeList->height = level - 1;

    Block *neighbour = getNeighbour(freeList);
    neighbour->height = level - 1;

    newBlock(neighbour);
    newBlock(freeList);

    return 0;
}

void mergeBuddy(Block *current) {
    if (current->height == buddyInfo->ablocks - 1) { 
        return;
    }

    Block *buddy_ptr = getNeighbour(current);

    if (current->height != buddy_ptr->height)
        return;

    if (current->allocated || buddy_ptr->allocated)
        return;

    removeBlock(current);
    removeBlock(buddy_ptr);

    if (current > buddy_ptr)
    {
        Block *aux = current;
        current = buddy_ptr;
        buddy_ptr = aux;
    }

    current->height++;
    newBlock(current);
    mergeBuddy(current);
}

#else 

typedef struct MemBlock {
	size_t size;
	struct MemBlock *next;
} MemBlock;

static MemBlock *alloc_mem = NULL;
static MemBlock *free_mem = NULL;

int initalizeMemoryManager(void *initialAddress, size_t size) {
	// Checks for an aligned address, size and enough memory to allocate.
	if(size < sizeof(MemBlock) + sizeof(size_t) || size % sizeof(size_t) != 0 || (size_t)initialAddress % sizeof(size_t) != 0)
		return 1;

	//Creates first block and assign it as the first element of the free memory list.
	MemBlock * first_block = initialAddress;
	first_block -> size = size - sizeof(MemBlock);
	first_block -> next = NULL;

	free_mem = first_block;

	infoMem.allocated = sizeof(MemBlock);
	infoMem.free = first_block -> size;
	infoMem.total = size;

	return 0;
}

void *malloc(size_t size) {
    printString((uint8_t *)"Malloc\n", WHITE);
    // Verificar si el tamaño solicitado es 0
    if (size == 0) return NULL;

    // Asegurarse de que el tamaño solicitado está alineado
    if (size % sizeof(size_t) != 0) {
        size += sizeof(size_t) - (size % sizeof(size_t));
    }

    // Buscar el primer bloque que sea lo suficientemente grande
    MemBlock *current = free_mem;


    MemBlock *previous = NULL;
    while (current != NULL && current->size < size) {
        previous = current;
        current = current->next;
    }

    // Si no se encuentra un bloque adecuado, devolver NULL
    if (current == NULL) return NULL;


    // Si el bloque es mucho más grande que el tamaño solicitado, dividirlo
    if (current->size >= size + MIN_BLOCK_SIZE) {
        MemBlock *newBlock = (MemBlock *)((char *)(current + 1) + size);
        newBlock->size = current->size - size - sizeof(MemBlock);
        newBlock->next = current->next;

        current->size = size;
        current->next = newBlock;

        infoMem.allocated += sizeof(MemBlock);
        infoMem.free -= sizeof(MemBlock);
    }

    // Eliminar el bloque de la lista de memoria libre
    if (previous != NULL) {
        previous->next = current->next;
    } else {
        free_mem = current->next;
    }
    infoMem.free -= current->size;

    // Agregar el bloque a la lista de memoria asignada
    current->next = alloc_mem;
    alloc_mem = current;
    infoMem.allocated += current->size;
    // Devolver un puntero al bloque asignado
    return (void *)(current + 1);
}

void free(void *ptr) {
    if (!ptr) return;  // Si el puntero es NULL, simplemente regresar

    // Convertir el puntero al bloque que lo precede
    MemBlock *blockToFree = (MemBlock *)ptr - 1;

    // Buscar el bloque en la lista de memoria asignada
    MemBlock *current = alloc_mem;
    MemBlock *previousAlloc = NULL;
    while (current != NULL && current != blockToFree) {
        previousAlloc = current;
        current = current->next;
    }

    // Si no se encuentra el bloque, terminar
    if (current == NULL) return;

    // Eliminar el bloque de la lista de memoria asignada
    if (previousAlloc) {
        previousAlloc->next = current->next;
    } else {
        alloc_mem = current->next;
    }

    // Insertar el bloque en la lista de memoria libre ordenada por dirección
    MemBlock *previousFree = NULL;
    current = free_mem;
    while (current != NULL && current < blockToFree) {
        previousFree = current;
        current = current->next;
    }

    if (previousFree) {
        blockToFree->next = previousFree->next;
        previousFree->next = blockToFree;
    } else {
        blockToFree->next = free_mem;
        free_mem = blockToFree;
    }

    // Actualizar la información de memoria
    infoMem.allocated -= blockToFree->size + sizeof(MemBlock);
    infoMem.free += blockToFree->size + sizeof(MemBlock);

    // Fusionar bloques contiguos
    current = free_mem;
    while (current && current->next) {
        if ((char *)(current + 1) + current->size == (char *)(current->next)) {
            current->size += sizeof(MemBlock) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

#endif

void getInfoMem(info_Mem *infoMemPtr) {
	//Le devuelvo la direccion de la estructura
	*infoMemPtr = infoMem;
    return;
}