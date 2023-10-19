#include <MemoryManager.h>

/* typedef struct MemoryManagerCDT {
	char *nextAddress;
} MemoryManagerCDT;

static MemoryManagerADT memory;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = managedMemory;

	memory = memoryManager;

	return memoryManager;
}

void *allocMemory(const uint64_t memoryToAllocate) {
	char *allocation = memory->nextAddress;

	memory->nextAddress += memoryToAllocate;

	return (void *) allocation;
} */

typedef struct MemBlock {
	size_t size;
	struct MemBlock *next;
} MemBlock;

MemBlock *free_mem = NULL;
MemBlock *alloc_mem = NULL;

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
	if(size == 0)
		return NULL;

	//First fit
	MemBlock *block = free_mem;
	while(block != NULL && block -> size < size) {
		block = block -> next;
	}

	//Llegamos al final de la lista de free_mem
	if(block == NULL)
		return NULL;

	
}