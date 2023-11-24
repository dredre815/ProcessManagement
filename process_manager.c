#define IMPLEMENTS_REAL_PROCESS
#include "process_manager.h"
#define MAX_MEMORY_SIZE 2048

/*
    * Read the command line arguments.
    * 
    * @param argc The number of command line arguments
    * @param argv The array of command line arguments
    * @param filename The path to the input file
    * @param scheduler The scheduling algorithm
    * @param memory_strategy The memory allocation strategy
    * @param quantum The quantum value
    */
void read_arguments(int argc, char *argv[], char **filename, Scheduler *scheduler, MemoryStrategy *memory_strategy, int *quantum) {
    int opt;
    // Parse command line arguments
    while ((opt = getopt(argc, argv, "f:s:m:q:")) != -1) {
        switch (opt) {
            case 'f':
                *filename = optarg;
                // Check if filename is provided
                if (*filename == NULL) {
                    fprintf(stderr, "Filename must be provided\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                if (strcmp(optarg, "SJF") == 0) {
                    *scheduler = SJF;
                } else if (strcmp(optarg, "RR") == 0) {
                    *scheduler = RR;
                } else {
                    // Check if scheduler is valid
                    fprintf(stderr, "Invalid scheduler option\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                if (strcmp(optarg, "infinite") == 0) {
                    *memory_strategy = INFINITE;
                } else if (strcmp(optarg, "best-fit") == 0) {
                    *memory_strategy = BEST_FIT;
                } else {
                    // Check if memory strategy is valid
                    fprintf(stderr, "Invalid memory strategy option\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'q':
                *quantum = atoi(optarg);
                // Check if quantum is valid
                if (*quantum < 1 || *quantum > 3) {
                    fprintf(stderr, "Invalid quantum value\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                // Check if all arguments are provided
                fprintf(stderr, "Usage: %s -f <filename> -s (SJF | RR) -m (infinite | best-fit) -q (1 | 2 | 3)\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

/*
    * Read the input file and store the processes in the array.
    * 
    * @param filename The path to the input file
    * @param processes The array of processes
    * @param process_count The number of processes
    */
void read_input_file(char *filename, Process **processes, int *process_count) {
    FILE *file = fopen(filename, "r");
    // Check if file exists
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    // Count the number of processes
    while (!feof(file)) {
        if (fgetc(file) == '\n') {
            (*process_count)++;
        }
    }
    
    // Reset the file pointer
    rewind(file);

    // Allocate memory for the processes
    *processes = (Process *)calloc(*process_count, sizeof(Process));
    // Read the processes from the file
    for (int i = 0; i < *process_count; i++) {
        fscanf(file, "%u %s %u %u\n", &(*processes)[i].time_arrived, (*processes)[i].process_name,
        &(*processes)[i].service_time, &(*processes)[i].memory_requirement);
        // Set the remaining time to the service time
        (*processes)[i].remaining_time = (*processes)[i].service_time;
    }
    fclose(file);
}

/*
    * Terminate the finished process and free the memory.
    *
    * @param current_running_process The current running process
    * @param current_time The current time
    * @param ready_queue The ready queue
    * @param input_queue The input queue
    * @param memory The head of the memory block
    * @param memory_strategy The memory allocation strategy
*/
void handle_finished_process(Process *current_running_process, int current_time, Node **ready_queue, Node **input_queue, mem_block_t **memory, MemoryStrategy memory_strategy) {
    // Terminate the process
    terminate_process(current_running_process->pid, current_running_process->pipe_fd[1], current_time,
    current_running_process->pipe_fd[0], current_running_process);

    int proc_remaining = count_process(*ready_queue) + count_process(*input_queue);
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", 
    current_time, current_running_process->process_name, proc_remaining);
    printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n",
    current_time, current_running_process->process_name, current_running_process->sha256_digest);
    if (memory_strategy == BEST_FIT) {
        // Free the memory
        release_mem(*memory, current_running_process);
    }
}

/*
    * Move the processes that have been submitted to the system to the input queue.
    * 
    * @param processes The array of processes
    * @param process_count The number of processes
    * @param input_queue The input queue
    * @param current_index The current index of the processes array
    * @param current_time The current time
    */
void move_process_to_input_queue(Process *processes, int process_count, Node **input_queue, int *current_index, int current_time) {
    int i;
    for (i = *current_index; i < process_count; i++) {
        if (processes[i].time_arrived <= current_time) {
            enqueue(input_queue, &processes[i]);
        } else {
            break;
        }
    }
    // Update the current index
    *current_index = i;
}

/*
    * Move the processes that successfully allocated memory to the ready queue.
    * 
    * @param input_queue The input queue
    * @param ready_queue The ready queue
    * @param memory_strategy The memory allocation strategy
    * @param memory The head of the memory block list
    * @param current_time The current time
    */
void move_process_to_ready_queue(Node **input_queue, Node **ready_queue, MemoryStrategy memory_strategy, mem_block_t **memory, int current_time) {
    Node *temp = *input_queue;
    while (temp) {
        Process *process = temp->process;
        if (process->time_arrived <= current_time) {
            if (memory_strategy == BEST_FIT) {
                mem_block_t *allocated_memory = best_fit_alloc(*memory, process, current_time);
                if (allocated_memory) {
                    enqueue(ready_queue, process);
                    dequeue(input_queue);
                } else {
                    // No memory available
                    break;
                }
            } else {
                enqueue(ready_queue, process);
                dequeue(input_queue);
            }
        } else {
            // No process is ready
            break;
        }
        temp = *input_queue;
    }
}

/*
    * print the statistics of the simulation.
    *
    * @param process_count The number of processes
    * @param total_turnaround_time The total turnaround time
    * @param max_time_overhead The maximum time overhead
    * @param total_time_overhead The total time overhead
    * @param current_time The current time
*/
void print_statistics(int process_count, double total_turnaround_time, double max_time_overhead, double total_time_overhead, int current_time) {
    max_time_overhead = round(max_time_overhead * 100) / 100;
    int average_turnaround_time = (int)(ceil(total_turnaround_time / process_count));
    double average_time_overhead = total_time_overhead / process_count;
    average_time_overhead = round(average_time_overhead * 100) / 100;

    // Print the statistics
    printf("Turnaround time %d\n", average_turnaround_time);
    printf("Time overhead %.2f %.2f\n", max_time_overhead, average_time_overhead);
    printf("Makespan %d\n", current_time);
}

/*
    * The main function.
    *
    * @param argc The number of command line arguments
    * @param argv The array of command line arguments
    * @return 0 if the program terminates successfully
*/
int main(int argc, char *argv[]) {
    char *filename = NULL;
    Scheduler scheduler;
    MemoryStrategy memory_strategy;
    int quantum;
    Process *processes = NULL;
    int process_count = 0;
    int process_count_finished = 0;
    Node *input_queue = NULL;
    Node *ready_queue = NULL;
    int current_time = 0;
    int current_index = 0;
    mem_block_t *memory = NULL;
    Process *current_running_process = NULL;
    double total_turnaround_time = 0;
    double max_time_overhead = 0;
    double total_time_overhead = 0;

    // Read the command line arguments
    read_arguments(argc, argv, &filename, &scheduler, &memory_strategy, &quantum);

    // Read the input file
    read_input_file(filename, &processes, &process_count);
    process_count_finished = process_count;

    // Initialize the memory for best fit
    if (memory_strategy == BEST_FIT) {
        memory = init_mem(MAX_MEMORY_SIZE);
    }

    // Start the simulation
    while (!is_empty(ready_queue) || current_running_process != NULL || process_count_finished != 0) {
        // check if the current running process has finished
        if (current_running_process != NULL && current_running_process->remaining_time <= 0) {
            double turnaround_time = current_time - current_running_process->time_arrived;
            double time_overhead = turnaround_time / current_running_process->service_time;
            if (time_overhead > max_time_overhead) {
                max_time_overhead = time_overhead;
            }
            total_time_overhead += time_overhead;
            total_turnaround_time += turnaround_time;
            // Terminate the process and free the memory
            handle_finished_process(current_running_process, current_time, &ready_queue, &input_queue, &memory, memory_strategy);
            current_running_process = NULL;
            // Count the number of finished processes
            process_count_finished -= 1;
            if (process_count_finished == 0) {
                break;
            }
        }

        // Move the processes that have been submitted to the system to the input queue
        move_process_to_input_queue(processes, process_count, &input_queue, &current_index, current_time);

        // Move the processes that successfully allocated memory to the ready queue
        move_process_to_ready_queue(&input_queue, &ready_queue, memory_strategy, &memory, current_time);

        // Run the scheduler
        if (scheduler == SJF) {
            current_running_process = shortest_job_first(&ready_queue, &current_time, quantum, current_running_process);
        } else {
            current_running_process = round_robin(&ready_queue, &current_time, quantum, current_running_process);
        }

        // Update the current time
        current_time += quantum;
    }

    // Print the statistics of the simulation
    print_statistics(process_count, total_turnaround_time, max_time_overhead, total_time_overhead, current_time);

    // Free the memory
    free_mem(memory);
    free(processes);
    return 0;
}