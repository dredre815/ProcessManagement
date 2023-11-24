#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "simulated_process.h"

// Memory block structure
typedef struct mem_block {
    unsigned int start;
    unsigned int size;
    struct mem_block *next;
    int is_allocated;
} mem_block_t;

// Memory block functions
mem_block_t *init_mem(unsigned int size);
void free_mem(mem_block_t *memory);
mem_block_t *best_fit_alloc(mem_block_t *memory, Process *process, int current_time);
void release_mem(mem_block_t *memory, Process *process);

#endif // MEMORY_H
