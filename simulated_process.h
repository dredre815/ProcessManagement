#ifndef SIMULATED_PROCESS_H
#define SIMULATED_PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_PROCESS_NAME_LENGTH 8

// Simulated process structure
typedef struct {
    unsigned int time_arrived;
    char process_name[MAX_PROCESS_NAME_LENGTH+1];
    unsigned int service_time;
    unsigned int memory_requirement;
    int remaining_time;
    unsigned int memory_start;
    int pipe_fd[2];
    pid_t pid;
    char sha256_digest[65];
} Process;

// Simulated process queue
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

// Function prototypes for simulated process queue
void enqueue(Node **head, Process *process);
Process *dequeue(Node **head);
bool is_empty(Node *head);
int count_process(Node *head);

#endif /* SIMULATED_PROCESS_H */