
# Multi-Threaded Directory Copier (MWCp)

This project implements a multi-threaded directory copier utility called 'MWCp' (Multi-Worker Copier) that uses a worker-manager approach to copy files and subdirectories in parallel.

## pthread_barrier_t plugin:

The barrier structure waits for all threads in the buffer until the buffer is filled to its maximum capacity. When the capacity is full, all threads start at the same time.

## File Details

### systemHelper.h and systemHelper.c
These files contain system-level helper functions. systemHelper.h includes function prototypes and necessary include statements.

### stack.h and stack.c
These files define and implement a stack data structure and its associated operations.

### main.c
This file contains the main() function, which is the entry point of the program. It initializes necessary components and starts the worker-manager system.

### makefile
This file contains the instructions for compiling the project. You can use it as follows:
```bash
make
```

## Usage

### Compilation
To compile the program, use the following command:
```bash
make
```

### Execution
To run the program, use the following command:
```bash
./MWCp <buffer_size> <number_of_workers> <source_directory> <destination_directory>
```

Example:
```bash
./MWCp 1024 4 /source /destination
```

## Features

* **Parallel Copying:** The program uses multiple worker threads to copy files and subdirectories in parallel.
* **Stack Data Structure:** A stack data structure is used to manage the copying operations.
* **High Performance:** The buffer size and number of workers parameters can be optimized to increase the efficiency of the copying operations.

## Configuration and Compilation Flags

The Makefile uses flags such as `-pthread` for threading support and `-Wall` for compiler warnings to aid in robust development.

## Commands and Functionalities

Detailed descriptions and usage of commands are as follows:

* **Buffer Size:** Specifies the size of the buffer used during copying operations.
* **Number of Workers:** Specifies the number of worker threads to be used for parallel copying operations.
* **Source Directory:** Specifies the source directory containing the files and directories to be copied.
* **Destination Directory:** Specifies the destination directory where the files and directories will be copied.

## Conclusion

This system demonstrates a robust approach to managing file operations across multiple clients in a concurrent environment, ensuring efficiency and reliability. Further enhancements will focus on integrating safe file handling features to promote security and data integrity.

* If you have any further information or questions about this sample, please feel free to get in touch. Happy coding! 🚀
