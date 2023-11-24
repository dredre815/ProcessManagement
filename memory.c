#include "memory.h"

/*
    * Initializes the memory with the given size.
    * 
    * @param size The size of the simulated memory
    * @return The head of the memory linked list
    */
mem_block_t *init_mem(unsigned int size) {
    mem_block_t *mem = (mem_block_t *) malloc(sizeof(mem_block_t));
    mem->start = 0;
    mem->size = size;
    mem->next = NULL;
    // mark the block as free
    mem->is_allocated = 0;
    return mem;
}

/*
    * Frees all memory blocks in the given memory.
    *
    * @param memory The head of the memory linked list
    */
void free_mem(mem_block_t *memory) {
    mem_block_t *temp = memory;
    while (temp) {
        mem_block_t *next = temp->next;
        free(temp);
        temp = next;
    }
}

/*
    * Simulates the best fit memory allocation algorithm.
    *
    * @param memory The head of the memory linked list
    * @param process The process to allocate memory for
    * @param current_time The current time
    * @return The memory block allocated for the process
    */
mem_block_t *best_fit_alloc(mem_block_t *memory, Process *process, int current_time) {
    unsigned int process_size = process->memory_requirement;
    const char *process_name = process->process_name;
    mem_block_t *best_fit = NULL;
    // The minimum difference between the block size and the process size
    unsigned int min_diff = UINT_MAX;

    mem_block_t *current = memory;

    // Find the best fit
    while (current) {
        unsigned int diff = current->size - process_size;

        if (!current->is_allocated && current->size >= process_size && diff < min_diff) {
            best_fit = current;
            min_diff = diff;
        }

        current = current->next;
    }

    // Allocate memory for the process
    if (best_fit) {
        mem_block_t *new_block = (mem_block_t *) malloc(sizeof(mem_block_t));
        new_block->start = best_fit->start + process_size;
        new_block->size = best_fit->size - process_size;
        new_block->is_allocated = 0;
        new_block->next = best_fit->next;

        best_fit->size = process_size;
        // mark the block as allocated
        best_fit->is_allocated = 1;
        best_fit->next = new_block;

        // Print the event
        printf("%u,READY,process_name=%s,assigned_at=%u\n", current_time, process_name, best_fit->start);
        process->memory_start = best_fit->start;
    }

    return best_fit;
}

/*
    * Releases the memory allocated to the process.
    * 
    * @param memory The head of the memory linked list
    * @param process The process to release memory for
    */
void release_mem(mem_block_t *memory, Process *process) {
    unsigned int start = process->memory_start;
    mem_block_t *current = memory;
    mem_block_t *prev = NULL;

    // Find the memory block allocated to the process
    while (current) {
        if (current->start == start) {
            break;
        }
        prev = current;
        current = current->next;
    }

    // The block is not found
    if (!current) {
        return;
    }

    // Mark the block as free
    current->is_allocated = 0;

    // Merge with previous block
    if (prev && !prev->is_allocated && prev->start + prev->size == current->start) {
        prev->size += current->size;
        prev->next = current->next;
        free(current);
        // Update the current pointer
        current = prev;
    }

    // Merge with next block
    if (current->next && !current->next->is_allocated && current->start + current->size == current->next->start) {
        mem_block_t *next = current->next;
        current->size += next->size;
        current->next = next->next;
        free(next);
    }
}