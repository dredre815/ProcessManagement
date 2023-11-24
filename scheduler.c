#include "scheduler.h"

/*
    * Simulate the shortest job first scheduling algorithm.
    * 
    * @param ready_queue The ready queue
    * @param current_time The current time
    * @param quantum The quantum
    * @param current_running_process The current running process
    * @return The current running process
    */
Process *shortest_job_first(Node **ready_queue, int *current_time, int quantum, Process *current_running_process) {
    if (current_running_process == NULL) {
        // do nothing if the ready queue is empty
        if (is_empty(*ready_queue)) {
            return NULL;
        } else {
            Node *prev = NULL, *shortest_process_prev = NULL;
            Node *current = *ready_queue;
            Process *shortest_process = NULL;

            // find the shortest process in the ready queue
            while (current) {
                Process *current_process = current->process;
                if ((!shortest_process || current_process->remaining_time < shortest_process->remaining_time ||
                    (current_process->remaining_time == shortest_process->remaining_time &&
                    (current_process->time_arrived < shortest_process->time_arrived ||
                    (current_process->time_arrived == shortest_process->time_arrived &&
                    strcmp(current_process->process_name, shortest_process->process_name) < 0))))) {
                    shortest_process = current_process;
                    shortest_process_prev = prev;
                }
                prev = current;
                current = current->next;
            }

            // remove the shortest process from the ready queue
            if (shortest_process_prev) {
                shortest_process_prev->next = shortest_process_prev->next->next;
            } else {
                *ready_queue = (*ready_queue)->next;
            }

            // create a new process for the shortest process
            current_running_process = shortest_process;
            current_running_process->pid = create_process(current_running_process->process_name, true,
            &current_running_process->pipe_fd[1], &current_running_process->pipe_fd[0], *current_time);
            // print the running process
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", *current_time,
            current_running_process->process_name, current_running_process->remaining_time);
            current_running_process->remaining_time -= quantum;
            return current_running_process;
        } 
    } 
    // continue running the current process
    resume_process(current_running_process->pid, current_running_process->pipe_fd[1],
    *current_time, current_running_process->pipe_fd[0]);
    current_running_process->remaining_time -= quantum;
    return current_running_process;
}

/*
    * Simulate the round robin scheduling algorithm.
    * 
    * @param ready_queue The ready queue
    * @param current_time The current time
    * @param quantum The quantum
    * @param current_running_process The current running process
    * @return The current running process
    */
Process *round_robin(Node **ready_queue, int *current_time, int quantum, Process *current_running_process) {
    if (current_running_process == NULL) {
        // do nothing if the ready queue is empty
        if (is_empty(*ready_queue)) {
            return NULL;
        } 
    } else {
        if (!is_empty(*ready_queue)) {
            // suspend the current running process and put it back to the ready queue
            suspend_process(current_running_process->pid, current_running_process->pipe_fd[1], *current_time);
            enqueue(ready_queue, current_running_process);
        } else {
            // continue running the current process
            resume_process(current_running_process->pid, current_running_process->pipe_fd[1],
            *current_time, current_running_process->pipe_fd[0]);
            current_running_process->remaining_time -= quantum;
            return current_running_process;
        }
    }

    current_running_process = dequeue(ready_queue);
    if (current_running_process->pid == 0) {
        // create a new process for the next process in the ready queue
        current_running_process->pid = create_process(current_running_process->process_name, true,
        &current_running_process->pipe_fd[1], &current_running_process->pipe_fd[0], *current_time);
    } else {
        // continue running the next process in the ready queue
        resume_process(current_running_process->pid, current_running_process->pipe_fd[1],
        *current_time, current_running_process->pipe_fd[0]);
    }

    // print the running process
    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", *current_time,
    current_running_process->process_name, current_running_process->remaining_time);
    current_running_process->remaining_time -= quantum;
    return current_running_process;
}