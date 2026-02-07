# 🚗 Parking Management System (TCP Socket Server)

A client-server system implemented in C for automated parking management. The project uses TCP sockets and I/O multiplexing (`select`) to handle simultaneous connections without using multithreading.

## 📋 Description

This project simulates the interaction between a parking system (server) and drivers (clients). The server tracks free/occupied spots, calculates parking costs based on duration, and processes payments.

### Key Features:
* **Concurrent Server:** Uses `select()` to handle multiple clients simultaneously within a single process.
* **TCP Protocol:** Reliable communication between client and server.
* **Business Logic:**
  * Random allocation of parking spots.
  * Identification based on license plate number.
  * Dynamic price calculation (based on UNIX time difference).
  * Payment system with a dedicated command and change calculation.

## 🛠️ Requirements

* Operating System: Linux / Unix (required for `sys/socket.h`, `netinet/in.h`, `unistd.h` libraries).
* Compiler: GCC.

## 🚀 Installation and Usage

### 1. Compilation

Open a terminal in the project directory and compile the sources:

```bash
# Compile Server
gcc server.c -o server

# Compile Client
gcc client.c -o client
