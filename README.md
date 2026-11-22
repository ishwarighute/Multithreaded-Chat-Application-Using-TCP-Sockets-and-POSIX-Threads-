# Multithreaded Chat Application

## Project Description
The Multithreaded Chat Application is a simple client-server communication system developed using C programming.

It uses TCP sockets for communication and POSIX threads (Pthreads) to handle multiple clients simultaneously.

## Objectives
1. To develop a chat application using client-server architecture.
2. To handle multiple clients using threads.
3. To broadcast messages to connected clients.
4. To implement graceful client disconnection.
5. To understand socket programming and multithreading.

## Technologies Used
- Programming Language: C
- Socket Programming: TCP
- Multithreading: POSIX Pthreads
- Compiler: GCC
- Editor: Visual Studio Code
- Operating System: Linux / Ubuntu

## Project Files
- server.c: Handles client connections and message broadcasting.
- client.c: Sends and receives chat messages.
- README.md: Project documentation.

## Compilation

Compile the server:

gcc server.c -o server -pthread

Compile the client:

gcc client.c -o client -pthread

## Execution

1. Start the server using ./server.
2. Open another terminal and execute ./client.
3. Open additional terminals to connect multiple clients.
4. Exchange messages between connected clients.
5. Type /quit to disconnect a client.

## Conclusion
This project demonstrates the use of TCP socket programming and multithreading to develop a simple chat application that supports multiple clients.