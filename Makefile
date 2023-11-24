# Set compiler and flags
CC = gcc
CFLAGS = -Wall -g
# Define target and object files
TARGET = allocate
OBJ = simulated_process.o real_process.o memory.o scheduler.o process_manager.o

# Default rule to build target
all: $(TARGET)

# Link object files to create target executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

# Compile object files
simulated_process.o: simulated_process.c simulated_process.h
	$(CC) $(CFLAGS) -c simulated_process.c

real_process.o: real_process.c real_process.h simulated_process.h
	$(CC) $(CFLAGS) -c real_process.c

memory.o: memory.c memory.h simulated_process.h
	$(CC) $(CFLAGS) -c memory.c

scheduler.o: scheduler.c scheduler.h simulated_process.h real_process.h
	$(CC) $(CFLAGS) -c scheduler.c

process_manager.o: process_manager.c process_manager.h simulated_process.h scheduler.h memory.h real_process.h
	$(CC) $(CFLAGS) -c process_manager.c

# Clean up object files and target executable
clean:
	rm -f *.o $(TARGET)
