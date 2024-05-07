#  Concurrent File Access System

This project implements a concurrent file access system designed to allow multiple clients to connect, access, modify, and archive files located in a directory on the server side. The system is divided into a server-side program `(neHosServer)` and a client-side program `(neHosClient)`.

## Server-Side Details

The `neHosServer` program is initiated with the directory to manage and the maximum number of clients it can handle simultaneously. It handles client connections by forking a new process for each client, thus ensuring that multiple clients can be served at the same time. The server maintains a log file to record client interactions and manages its child processes effectively, especially in dealing with kill signals either from the server's command prompt or from a client request.

### Usage

```bash
	
	./neHosServer <dirname> <max. #ofClients>

```

## Client-Side Details

The `neHosClient` allows for various operations such as uploading and downloading files, reading from and writing to files, and archiving the server's directory. It supports commands like `help`, `list`, `upload`, `download`, `archServer`, and more. The client can connect to the server using the server's PID and performs tasks based on available spots in the server's queue.


### Usage

```bash
	
	./neHosClient <Connect/tryConnect> <server process id>

```

## Features:

* File Operations: The system supports operations on any file type and size, handling large files (over 10 MB) and various formats (e.g., text, binary).
* Archiving: Using the archServer command, clients can archive the current server directory into a tar file, leveraging multi-process handling with fork and exec.
* Robust Connection Handling: The server can manage a full queue of connections and gracefully handles disconnects and reconnections.

## Integration Issues:

Currently, the safe file functions are not fully integrated into the system. Efforts are ongoing to ensure these functionalities are seamlessly incorporated to enhance data security and integrity.


## Makefile and Compilation Flags

The project includes a Makefile to compile both the server and client programs. The Makefile uses flags such as `-pthread` for threading support and `-Wall` for compiler warnings to aid in robust development.

## Commands and Functionalities

Detailed descriptions and usage of commands are outlined in the provided `midterm2024.pdf` document. This includes how to perform file uploads, downloads, server archiving, and more.

## Conclusion

This system demonstrates a robust approach to managing file operations across multiple clients in a concurrent environment, ensuring efficiency and reliability. Further enhancements will focus on integrating safe file handling features to promote security and data integrity.


* If you have any further information or questions about this sample, please feel free to get in touch. Good coding! 🚀