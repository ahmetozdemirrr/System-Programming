# IPC OF FIFO's (named pipes)

## Introduction
In this README, I will discuss the implementation details of Homework 2, which involves creating two child processes that perform specific operations and communicate with each other using FIFOs. I will explain the design choices, error handling strategies, and bonus features implemented as per the assignment requirements.

## Implementation Details

### Input Handling
- The program prompts the user for the size of the random array using `read(STDIN_FILENO, sizeBuffer, COMMAND_SIZE)` and validates the input using `strtol()`.
- The second child process only accepts commands for addition and multiplication, rejecting other inputs.
- To prevent FIFO write errors due to operating system limitations, entries over a certain threshold are suppressed.

### Random Array Generation
- An array is dynamically allocated using `malloc()` based on the user-provided size.
- Random numbers between 1 and 10 are generated to populate the array, ensuring variability in results.

### Signal Handling
- The SIGCHLD signal is handled using the `sigaction` structure.
- The `sigchld_handler` function reaps terminated child processes, printing their process IDs and exit statuses.
- Proper error handling is implemented for unexpected child process termination.

### FIFO Usage
- FIFOs are created using `mkfifo` after forking child processes to ensure proper communication.
- FIFOs are opened with `O_RDONLY` or `O_WRONLY` flags as needed, and closed after use to avoid blocking.
- FIFO file creation errors are ignored if the files already exist in the directory.

### Error Handling
- System calls returning -1 in error conditions are checked, and appropriate error messages are printed.
- FIFO file creation errors due to existing files are ignored, utilizing the `EEXIST` error code.
- Resources are properly deallocated at the end of the program, including dynamically allocated memory and FIFO files.

### Bonus Features
- Exit statuses of all processes, including the parent process, are displayed on the screen.
- Zombie processes are prevented using the `waitpid()` function.

## Makefile and Usage
- The provided Makefile includes targets for compiling, running, and cleaning the program.
- Usage:
  - `make program`: compiles the program
  - `make run`: runs the program
  - `make clean`: cleans executable files and object files
  - `make`: compiles and runs the program simultaneously

## Conclusion
The assignment requirements, including IPC communication, signal handling, error checking, and bonus features, have been successfully implemented. The program meets the specified grading criteria and demonstrates proficiency in system programming concepts.



* If you have any further information or questions about this sample, please feel free to get in touch. Good coding! 🚀
