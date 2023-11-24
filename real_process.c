#include "real_process.h"

/*
    * Send the simulation time to the process.
    * 
    * @param pipe_fd The pipe file descriptor
    * @param simulation_time The simulation time
*/
void send_simulation_time(int pipe_fd, uint32_t simulation_time) {
    // convert the simulation time to big endian
    uint32_t big_endian_time = htonl(simulation_time);
    write(pipe_fd, &big_endian_time, sizeof(big_endian_time));
}

/*
    * Read a byte from the standard output of the process and verify it.
    * 
    * @param pipe_fd The pipe file descriptor
    * @param simulation_time The simulation time
    * @return 1 if the byte is correct, 0 otherwise
*/
uint8_t read_and_verify_byte(int pipe_fd, uint32_t simulation_time) {
    uint8_t byte_received;
    read(pipe_fd, &byte_received, sizeof(byte_received));
    return byte_received == (simulation_time & 0xFF);
}

/*
    * Create a process and return its process ID.
    * 
    * @param process_name The name of the process
    * @param verbose Whether to run the process in verbose mode
    * @param to_child_pipe The pipe to write to the child process
    * @param from_child_pipe The pipe to read from the child process
    * @param simulation_time The simulation time
    * @return The process ID of the child process
*/
pid_t create_process(const char *process_name, bool verbose, int *to_child_pipe, int *from_child_pipe, uint32_t simulation_time) {
    int pipe_in[2], pipe_out[2];

    // Create the pipes
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();

    if (pid == 0) { 
        // Child process
        close(pipe_in[1]);
        close(pipe_out[0]);
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        // Run the process
        execl("./process", "process", verbose ? "-v" : NULL, process_name, NULL);
        perror("execl");
        _exit(1);
    } else if (pid > 0) { 
        // Parent process
        close(pipe_in[0]);
        close(pipe_out[1]);
        *to_child_pipe = pipe_in[1];
        *from_child_pipe = pipe_out[0];
        send_simulation_time(*to_child_pipe, simulation_time);
        // Verify that the process started correctly
        if (!read_and_verify_byte(*from_child_pipe, simulation_time)) {
            fprintf(stderr, "Error: The process did not start correctly.\n");
            exit(1);
        }
        return pid;
    } else {
        perror("fork");
        return -1;
    }
}

/*
    * Suspend the process and wait for it to be resumed.
    * 
    * @param process_id The process ID of the process
    * @param pipe_fd The pipe file descriptor
    * @param simulation_time The simulation time
*/
void suspend_process(pid_t process_id, int pipe_fd, uint32_t simulation_time) {
    send_simulation_time(pipe_fd, simulation_time);
    kill(process_id, SIGTSTP);

    int wstatus;
    // Wait for the process to be stopped
    waitpid(process_id, &wstatus, WUNTRACED);
    while (!WIFSTOPPED(wstatus)) {
        waitpid(process_id, &wstatus, WUNTRACED);
    }
}

/*
    * Resume the process and verify that the correct byte is received.
    * 
    * @param process_id The process ID of the process
    * @param pipe_fd The pipe file descriptor
    * @param simulation_time The simulation time
    * @param from_child_pipe The pipe to read from the child process
*/
void resume_process(pid_t process_id, int pipe_fd, uint32_t simulation_time, int from_child_pipe) {
    send_simulation_time(pipe_fd, simulation_time);
    kill(process_id, SIGCONT);

    // Verify that the correct byte is received
    uint8_t received_byte = read_and_verify_byte(from_child_pipe, simulation_time);
    if (!received_byte) {
        fprintf(stderr, "Mismatched byte received from process %d\n", process_id);
    }
}

/*
    * Terminate the process and wait for it to terminate.
    * 
    * @param process_id The process ID of the process
    * @param pipe_fd The pipe file descriptor
    * @param simulation_time The simulation time
    * @param from_child_pipe The pipe to read from the child process
    * @param current_running_process The process that is currently running
*/
void terminate_process(pid_t process_id, int pipe_fd, uint32_t simulation_time, int from_child_pipe, Process *current_running_process) {
    char sha256[65];

    send_simulation_time(pipe_fd, simulation_time);

    // Terminate the child process
    kill(process_id, SIGTERM);

    // Read the SHA256 digest from the child process
    read(from_child_pipe, sha256, 64);
    sha256[64] = '\0';

    strcpy(current_running_process->sha256_digest, sha256);
}