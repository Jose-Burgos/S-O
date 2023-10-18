#include <MemoryManager.h>

typedef struct MemoryManagerCDT {
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
}
