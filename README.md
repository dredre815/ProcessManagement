# COMP30023: Computer Systems - Project 1: Process Management

## Overview

This project involves implementing a process manager for memory allocation and process scheduling. The project is divided into two phases: 
1. **Scheduling Algorithm**: Allocates CPU to processes under the assumption that memory requirements are always met.
2. **Memory Allocation**: Involves allocating memory to processes before scheduling.

Both memory allocation and process scheduling are simulated, with an additional challenge task involving real processes.

## Process Manager

- Operates in cycles, with each cycle following a quantum.
- Manages processes through a series of tasks:
  1. Check and terminate completed processes.
  2. Add new processes to the input queue.
  3. Move processes to the ready queue upon successful memory allocation.
  4. Determine which process runs in the current cycle.

## Programming Tasks

1. **Process Scheduling (Non-preemptive)**: Implement Shortest Job First (SJF) scheduling algorithm.
2. **Process Scheduling (Preemptive)**: Implement Round Robin (RR) scheduling algorithm.
3. **Memory Allocation**: Simulate memory allocation using a Best Fit algorithm.
4. **Challenge Task (Optional)**: Control real processes during scheduling.

## Program Specification

- The program, named `allocate`, accepts command line arguments specifying the filename, scheduler type, memory strategy, and quantum.
- The file format for process description is specified.
- Expected behavior for program execution is detailed.

## Expected Output

- Details on the format of execution transcript and performance statistics are provided.

## Marking Criteria

The project is evaluated based on:
1. Implementation of SJF and RR algorithms.
2. Best Fit memory allocation.
3. Controlling real processes (optional).
4. Computation of performance statistics.
5. Build quality.
6. Quality of software practices.

## Submission

- Code must be written in C and follow specified guidelines.
- Submission involves providing the SHA1 hash of the commit and pushing to a designated repository.

## Testing

- Several test cases are provided, but exhaustive testing is encouraged.

## Collaboration and Plagiarism

- Strict guidelines on individual work and academic integrity are emphasized.