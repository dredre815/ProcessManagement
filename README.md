# Process Management Simulator

## Overview

This project implements a process management system that simulates CPU scheduling and memory allocation. It supports both simulated and real process management with different scheduling algorithms and memory allocation strategies.

### Key Features

- Multiple scheduling algorithms (SJF, RR)
- Memory allocation strategies (Infinite, Best-Fit)
- Real process management with IPC
- Performance statistics tracking
- Configurable quantum time

## Technical Details

### Scheduling Algorithms

1. **Shortest Job First (SJF)**
   - Non-preemptive scheduling
   - Selects process with shortest remaining execution time
   - Handles ties using arrival time and process name

2. **Round Robin (RR)**
   - Preemptive scheduling
   - Time quantum based execution
   - Fair CPU distribution among processes

### Memory Management

- **Best Fit Algorithm**
  - Efficient memory allocation
  - Memory block splitting and merging
  - Maximum memory size: 2048 units
  - Dynamic memory block tracking

### Process States

Processes can be in one of these states:
- Input Queue (newly arrived)
- Ready Queue (memory allocated)
- Running (being executed)
- Finished (completed execution)

## Usage

### Compilation

```bash
make
```

### Running the Program

```bash
./allocate -f <filename> -s <scheduler> -m <memory-strategy> -q <quantum>
```

### Parameters

- `-f`: Input file path
- `-s`: Scheduler type (SJF | RR)
- `-m`: Memory strategy (infinite | best-fit)
- `-q`: Quantum value (1-3)

### Input File Format

Each line represents a process with the format:
```
<arrival_time> <process_name> <service_time> <memory_requirement>
```

## Output Format

### Process Events

The simulator outputs events in the following format:
```
<time>,<event_type>,process_name=<name>,<additional_info>
```

Event types include:
- READY
- RUNNING
- FINISHED

### Performance Statistics

At the end of simulation, outputs:
- Average turnaround time
- Time overhead (max and average)
- Total makespan

## Implementation Details

The project is structured into several key components:

1. **Process Manager**
   - Central coordinator
   - Handles process lifecycle
   - Manages scheduling and memory allocation

2. **Memory Manager**
   - Implements best-fit allocation
   - Handles memory block management
   - Tracks memory usage

3. **Scheduler**
   - Implements SJF and RR algorithms
   - Manages process queues
   - Handles context switching

4. **Real Process Handler**
   - Manages actual process creation
   - Handles IPC through pipes
   - Process synchronization

## Building and Testing

### Requirements

- GCC compiler
- Make build system
- POSIX-compliant system

### Build Process

1. Clone the repository
2. Run `make` to build
3. Run tests using provided test cases

## License

This project is licensed under the MIT License - see the LICENSE file for details.