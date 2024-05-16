# Parking Lot Simulation with Semaphores

This project simulates a parking lot where two parking attendants manage parking spaces for pickups and automobiles. The simulation involves threads, semaphores, and shared memory to handle the synchronization between vehicle arrivals and parking attendants.

## Project Description

Two parking attendants supervise a parking lot with a capacity of four spaces for pickups and eight spaces for automobiles. Various vehicles (automobiles and pickups) arrive at the parking lot entrance, determined by a random generator. Each parking attendant is responsible for a specific type of vehicle. The synchronization of these threads is achieved using semaphores.

### Files

- `systemHelper.h`: Header file containing declarations and macros.
- `systemHelper.c`: Implementation of helper functions and thread functions.
- `system.c`: Main program to initialize, run, and clean up the simulation.
- `makefile`: Makefile to compile and manage the project.

## Compilation and Execution

### Prerequisites

- GCC compiler
- Make utility
- Valgrind (for memory leak detection)
- Helgrind (for thread error detection)

### Build the Project

To compile the project, run the following command in the terminal:

```sh
make all
```

or

```sh
make
```

To running programme:

```
./system
```

To cleaning temporary file:

```sh
make clean
```

To mem-check:

```sh
make valgrind
```

To thread-safe control (on race condition):

```sh
make helgrind
```

Sample output:


```
ahmete@ahmete-Inspiron-14-5401:~/System-Programming/Parking-System-Simulation-with-Thread$ make
ahmete@ahmete-Inspiron-14-5401:~/System-Programming/Parking-System-Simulation-with-Thread$ ./system
+ Pickup parked. Available empty automobile spaces: 8 pickup spaces: 3
+ Automobile parked. Available empty automobile spaces: 7 pickup spaces: 3
+ Pickup parked. Available empty automobile spaces: 7 pickup spaces: 2
+ Pickup parked. Available empty automobile spaces: 7 pickup spaces: 1
+ Pickup parked. Available empty automobile spaces: 7 pickup spaces: 0
x There is no empty space for pickup.
- The pickup has been removed. Number of available free pickup parking spaces: 1
+ Automobile parked. Available empty automobile spaces: 6 pickup spaces: 1
- The automobile has been removed. Number of available free automobile parking spaces: 7
+ Automobile parked. Available empty automobile spaces: 6 pickup spaces: 1
+ Pickup parked. Available empty automobile spaces: 6 pickup spaces: 0
x There is no empty space for pickup.
+ Automobile parked. Available empty automobile spaces: 5 pickup spaces: 0
x There is no empty space for pickup.
- The pickup has been removed. Number of available free pickup parking spaces: 1
+ Automobile parked. Available empty automobile spaces: 4 pickup spaces: 1
- The automobile has been removed. Number of available free automobile parking spaces: 5
+ Pickup parked. Available empty automobile spaces: 5 pickup spaces: 0
x There is no empty space for pickup.
+ Automobile parked. Available empty automobile spaces: 4 pickup spaces: 0
+ Automobile parked. Available empty automobile spaces: 3 pickup spaces: 0
+ Automobile parked. Available empty automobile spaces: 2 pickup spaces: 0
- The pickup has been removed. Number of available free pickup parking spaces: 1
+ Automobile parked. Available empty automobile spaces: 1 pickup spaces: 1
- The automobile has been removed. Number of available free automobile parking spaces: 2
+ Automobile parked. Available empty automobile spaces: 1 pickup spaces: 1
+ Pickup parked. Available empty automobile spaces: 1 pickup spaces: 0
+ Automobile parked. Available empty automobile spaces: 0 pickup spaces: 0
x There is no empty space for pickup.
x There is no empty space for pickup.
- The pickup has been removed. Number of available free pickup parking spaces: 1
x There is no empty space for automobile.
- The automobile has been removed. Number of available free automobile parking spaces: 1
+ Automobile parked. Available empty automobile spaces: 0 pickup spaces: 1
x There is no empty space for automobile.
+ Pickup parked. Available empty automobile spaces: 0 pickup spaces: 0
x There is no empty space for pickup.
x There is no empty space for pickup.
* Terminating the programme...
ahmete@ahmete-Inspiron-14-5401:~/System-Programming/Parking-System-Simulation-with-Thread$ 

```

* If you have any further information or questions about this sample, please feel free to get in touch. Good coding! 🚀