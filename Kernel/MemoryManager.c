#include <MemoryManager.h>
#include <stddef.h> 

typedef struct MemBlock {
	size_t size;
	struct MemBlock *next;
} MemBlock;

typedef struct {
    size_t allocated;
    size_t free;
    size_t total;
} MemoryInfo;

MemoryInfo infoMem;

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
    if (current->size >= size + sizeof(MemBlock) + MIN_BLOCK_SIZE) {
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

void getInfoMem(info_Mem *infoMemPtr) {
	//Le devuelvo la direccion de la estructura
	*infoMemPtr = infoMem;
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

    // Actualizar la información de memoria
    infoMem.allocated -= blockToFree->size + sizeof(MemBlock);
    infoMem.free += blockToFree->size + sizeof(MemBlock);
}