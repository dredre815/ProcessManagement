#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "simulated_process.h"
#include "real_process.h"

// Scheduler functions
Process *shortest_job_first(Node **ready_queue, int *current_time, int quantum, Process *current_running_process);
Process *round_robin(Node **ready_queue, int *current_time, int quantum, Process *current_running_process);

#endif // SCHEDULER_H