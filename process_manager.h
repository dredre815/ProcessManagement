#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "simulated_process.h"
#include "scheduler.h"
#include "memory.h"
#include "real_process.h"

// Two types of scheduling algorithms
typedef enum {
    SJF,
    RR
} Scheduler;

// Two types of memory allocation strategies
typedef enum {
    INFINITE,
    BEST_FIT
} MemoryStrategy;

// Process manager functions
void read_arguments(int argc, char *argv[], char **filename, Scheduler *scheduler, MemoryStrategy *memory_strategy, int *quantum);
void read_input_file(char *filename, Process **processes, int *process_count);
void handle_finished_process(Process *current_running_process, int current_time, Node **ready_queue, Node **input_queue, mem_block_t **memory, MemoryStrategy memory_strategy);
void move_process_to_input_queue(Process *processes, int process_count, Node **input_queue, int *current_index, int current_time);
void move_process_to_ready_queue(Node **input_queue, Node **ready_queue, MemoryStrategy memory_strategy, mem_block_t **memory, int current_time);
void print_statistics(int process_count, double total_turnaround_time, double max_time_overhead, double total_time_overhead, int current_time);

#endif // PROCESS_MANAGER_H