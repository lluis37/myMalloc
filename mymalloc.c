#include <stdio.h>
#include <stdbool.h>
#include "mymalloc.h"

// Structure to represent each chunk of memory managed by mymalloc/myfree.
typedef struct CHUNK_DATA {
    size_t size; // Size of the chunk excluding the size of the metadata.
    int free; // Flag to indicate if the chunk is free (1) or allocated (0).
    struct CHUNK_DATA *next; // Pointer to the next chunk. Can be used for free list management.
} CHUNK_DATA;

#define MEMLENGTH 512 // Size of the memory pool in terms of `double` units.
#define CHUNKSIZE sizeof(CHUNK_DATA) // Size of the metadata of each chunk.

#define ALIGNMENT 8 // Memory alignment.
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1)) // Aligns the size to the nearest multiple of ALIGNMENT.

static double memory[MEMLENGTH]; // The static memory pool.
#define heapStart ((void*)memory) // Starting address of the memory pool.

// Initializes the memory pool.
void init_mem() {
    CHUNK_DATA *head = (CHUNK_DATA*)heapStart;
    head->size = MEMLENGTH * sizeof(double) - CHUNKSIZE; // Initial chunk spans the entire pool minus metadata size.
    head->free = 1; // The initial chunk is free.
    head->next = NULL; // No next chunk.
}

// Splits a chunk into two parts if the size is sufficient.
void split(CHUNK_DATA *fitting_slot, size_t size) {
    // Create a new chunk in the memory right after the allocated space.
    CHUNK_DATA *new = (void*)((char*)fitting_slot + size + CHUNKSIZE);
    new->size = fitting_slot->size - size - CHUNKSIZE; // Size of the new chunk after split.
    new->free = 1; // New chunk is free.
    new->next = fitting_slot->next; // Link the new chunk to the next chunk.
    fitting_slot->size = size; // Update the size of the allocated chunk.
    fitting_slot->free = 0; // Mark the allocated chunk as not free.
    fitting_slot->next = new; // Link the allocated chunk to the new chunk.
}

// Allocates a chunk of memory of at least 'size' bytes.
void *mymalloc(size_t size, char *file, int line) {
    // Initialize memory pool on first allocation.
    if (((CHUNK_DATA*)heapStart)->size == 0) {
        init_mem();
        printf("Memory initialized. Heap start at: %p, Size: %zu\n", (void*)memory, (size_t)MEMLENGTH * sizeof(double));
    }

    size = ALIGN(size); // Align the requested size to the alignment boundary.

    // Start searching for a free chunk from the beginning of the memory pool.
    CHUNK_DATA *current = (CHUNK_DATA*)heapStart;

    // Find a free chunk that is large enough to accommodate the requested size.
    while ((current->size < size || current->free == 0) && current->next != NULL) {
        current = current->next;
    }

    // If no suitable chunk was found, return NULL.
    if (current->size < size || current->free == 0) {
        fprintf(stderr, "Allocation error: no sufficient memory available (%s:%d)\n", file, line);
        return NULL;
    }

    // If the chunk is larger than needed, split it.
    if (current->size > (size + CHUNKSIZE)) {
        split(current, size);
    } else {
        current->free = 0; // Otherwise, just mark it as allocated.
    }

    // Return a pointer to the usable memory, just after the metadata.
    return (void*)((char*)current + CHUNKSIZE);
}

// Merges consecutive free chunks to form larger chunks.
void coalesce() {
    CHUNK_DATA *current = (CHUNK_DATA*)heapStart;
    while (current != NULL && current->next != NULL) {
        if (current->free && current->next->free) {
            current->size += current->next->size + CHUNKSIZE; // Merge the next chunk into current.
            current->next = current->next->next; // Skip over the merged chunk.
        } else {
            current = current->next; // Move to the next chunk.
        }
    }
}

// Frees a previously allocated chunk.
void myfree(void *ptr, char *file, int line) {
    // Do nothing if a NULL pointer is passed.
    if (!ptr) {
        fprintf(stderr, "myfree error: NULL pointer passed (file: %s, line: %d)\n", file, line);
        return;
    }

    // Pointer validation: ensure it points to the start of an allocated block.
    CHUNK_DATA *current = (CHUNK_DATA*)((char*)memory);
    bool found = false;

    // Iterate through the memory pool to find the matching chunk.
    while ((void*)current < (void*)memory + MEMLENGTH * sizeof(double)) {
        void* chunkEnd = (void*)((char*)current + CHUNKSIZE + current->size);
        if (ptr >= (void*)current && ptr < chunkEnd) {
            if ((void*)((char*)current + CHUNKSIZE) == ptr && current->free == 0) {
                found = true; // Pointer is valid and points to the start of an allocated chunk.
                break;
            } else {
                // This else is reached if the pointer does not point to the start of an allocated chunk.
                // It covers two scenarios:
                // 1. The pointer is within the boundaries of an allocated chunk but not at its start.
                // 2. The pointer points to the start of a chunk that is already marked as free.
                fprintf(stderr, "myfree error: Invalid pointer (file: %s, line: %d)\n", file, line);
                return;
            }

        }
        // Move to the next chunk in the pool.
        current = (void*)((char*)current + CHUNKSIZE + current->size);
    }

    if (!found) {
        // The pointer was not found in any chunk (it may be outside the memory pool).
        fprintf(stderr, "myfree error: Pointer not found (file: %s, line: %d)\n", file, line);
        return;
    }

    // Mark the found chunk as free.
    current->free = 1;
    // Try to merge this free chunk with adjacent free chunks.
    coalesce();
}
