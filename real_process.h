#ifndef REAL_PROCESS_H
#define REAL_PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "simulated_process.h"

// Function prototypes for real process
void send_simulation_time(int pipe_fd, uint32_t simulation_time);
uint8_t read_and_verify_byte(int pipe_fd, uint32_t simulation_time);
pid_t create_process(const char *process_name, bool verbose, int *to_child_pipe, int *from_child_pipe, uint32_t simulation_time);
void suspend_process(pid_t process_id, int pipe_fd, uint32_t simulation_time);
void resume_process(pid_t process_id, int pipe_fd, uint32_t simulation_time, int from_child_pipe);
void terminate_process(pid_t process_id, int pipe_fd, uint32_t simulation_time, int from_child_pipe, Process *current_running_process);

#endif // REAL_PROCESS_H
