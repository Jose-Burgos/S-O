#include <MemoryManager.h>
#include <videoDriver.h>

static info_Mem infoMem;

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

static BuddyInfo * buddy;

size_t log2(size_t n); 
void *getAddress(Block *block_ptr);
Block *getBuddy(Block *block_ptr);
void addBlock(Block *block_ptr);
void removeBlock(Block *block_ptr);
int splitBlock(size_t level);
void mergeBuddy(Block *block_ptr);

int initalizeMemoryManager(void *initialAddress, size_t size)
{
    if ((size_t)initialAddress % sizeof(size_t) != 0 || size % BLOCK_SIZE != 0 || !IS_POW2(size / BLOCK_SIZE)) return -1;

    size_t level = log2(size / BLOCK_SIZE);

    if (level == 0 || level >= TREE_HEIGHT) return -1;

    infoMem.allocated = 0;
    infoMem.total = size - sizeof(BuddyInfo);
    infoMem.free = size - sizeof(BuddyInfo);
    buddy = initialAddress;

    for (size_t i = 0; i < BLOCK_LIMIT; i++) {
        buddy->blocks[i].size = 0;
        buddy->blocks[i].level = 0;
        buddy->blocks[i].next = NULL;
        buddy->blocks[i].prev = NULL;
    }

    for (size_t i = 0; i < TREE_HEIGHT; i++) {
        buddy-> freeRow[i] = NULL;
    }

    buddy->blocks[0].level = level;
    buddy->freeRow[level] = buddy->blocks;
    buddy->lvl = level + 1;
    buddy->used = 1 << level;
    buddy->start = initialAddress + sizeof(BuddyInfo);

    return 0;
}


void *malloc(size_t size) {
    size_t level = log2(size / BLOCK_SIZE);
    if ((1 << level) * BLOCK_SIZE < size) {
        level++;
    }

    if (buddy->freeRow[level] == NULL)
    {
        size_t min_above = level + 1;
        while (min_above < buddy->lvl && buddy->freeRow[min_above] == NULL) {
            min_above++;
        }

        if (min_above == buddy->lvl) return NULL;

        while (min_above > level) {
            splitBlock(min_above--);
        }
    }

    Block *block_ptr = buddy->freeRow[level];
    removeBlock(block_ptr);
    return getAddress(block_ptr);
}

void free(void *ptr) {
    size_t offset = ptr - buddy->start;
    if (offset % BLOCK_SIZE) return;

    Block *block_ptr = buddy->blocks + (offset / BLOCK_SIZE);
    if (block_ptr >= buddy->blocks + buddy->used || !block_ptr->size) return;

    addBlock(block_ptr);
    mergeBuddy(block_ptr);
}

size_t log2(size_t n) {
    size_t ret = 0;
    while (n >>= 1)
        ++ret;
    return ret;
}

void *getAddress(Block *block_ptr) {
    return buddy->start + (block_ptr - buddy->blocks) * BLOCK_SIZE;
}

Block *getBuddy(Block *block_ptr) {
    size_t level_index = (block_ptr - buddy->blocks) >> block_ptr->level;
    if (level_index % 2) {
        return block_ptr - (1 << block_ptr->level);
    }
    else {
        return block_ptr + (1 << block_ptr->level);
    }
}

void addBlock(Block *block_ptr) {
    block_ptr->next = buddy->freeRow[block_ptr->level];
    block_ptr->prev = NULL;

    buddy->freeRow[block_ptr->level] = block_ptr;

    if (block_ptr->next != NULL) {
        block_ptr->next->prev = block_ptr;
    }

    block_ptr->size = 0;
    infoMem.allocated -= SIZE(block_ptr->level);
    infoMem.free += SIZE(block_ptr->level);
}

void removeBlock(Block *block_ptr) {
    if (block_ptr->next) {
        block_ptr->next->prev = block_ptr->prev;
    }

    if (block_ptr->prev != NULL) {
        block_ptr->prev->next = block_ptr->next;
    } else {
        buddy->freeRow[block_ptr->level] = block_ptr->next;
    }

    block_ptr->next = NULL;
    block_ptr->prev = NULL;
    block_ptr->size = SIZE(block_ptr->level);
    infoMem.allocated += block_ptr->size;
    infoMem.free -= block_ptr->size;
}

int splitBlock(size_t level) {
    if (level == 0 || buddy->freeRow[level] == NULL) return -1;

    Block *block_ptr = buddy->freeRow[level];
    removeBlock(block_ptr);
    block_ptr->level = level - 1;

    Block *buddy_ptr = getBuddy(block_ptr);
    buddy_ptr->level = level - 1;

    addBlock(buddy_ptr);
    addBlock(block_ptr);

    return 0;
}

void mergeBuddy(Block *block_ptr) {
    if (block_ptr->level == buddy->lvl - 1) return;

    Block *buddy_ptr = getBuddy(block_ptr);

    if (block_ptr->level != buddy_ptr->level) return;

    if (!block_ptr->size || !buddy_ptr->size) return;

    removeBlock(block_ptr);
    removeBlock(buddy_ptr);

    if (block_ptr > buddy_ptr)
    {
        Block *aux = block_ptr;
        block_ptr = buddy_ptr;
        buddy_ptr = aux;
    }

    block_ptr->level++;
    addBlock(block_ptr);
    mergeBuddy(block_ptr);
}

#else 
#define MIN_BLOCK_SIZE sizeof(MemBlock) + 100

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