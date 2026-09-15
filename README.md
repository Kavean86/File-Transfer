# File Transfer

A lightweight file transfer application written in **C++** that uses **TCP sockets** and **SSH** to transfer files between Linux systems.

The project is being developed from scratch to explore low-level network programming, socket communication, process management, and reliable file transmission on Linux.

## Features

* 📁 File transfer between Linux systems
* 🔌 TCP socket communication
* 🔐 SSH-based remote connection
* ⚙️ Process management using `fork()` and `exec()`
* 📦 Binary file transfer
* 💾 Buffered file reading and writing
* 🖥️ Command-line based
* 🐧 Designed for Linux environments

## How It Works

The application uses a client/server architecture.

A simplified transfer flow looks like this:

```text
Client
  │
  │  SSH connection
  ▼
Remote system
  │
  │  Start receiver
  ▼
Receiver
  │
  │  TCP connection
  ▼
Sender
  │
  │  File data
  ▼
Receiver
  │
  ▼
Output file
```

The project also uses Linux process-management functions to start remote commands and establish the required communication processes.

## Technologies

* **C++**
* **Linux**
* **TCP/IP**
* **POSIX Sockets**
* **SSH**
* `fork()`
* `exec()`
* `connect()`
* `bind()`
* `listen()`
* `accept()`
* `send()`
* `recv()`
* File streams

## Requirements

A Linux environment with:

* C++ compiler
* POSIX-compatible operating system
* OpenSSH client/server
* Git
* Bash

For example, on Debian/Ubuntu:

```bash
sudo apt update
sudo apt install g++ openssh-client openssh-server git
```

## Build

Clone the repository:

```bash
git clone https://github.com/Kavean86/File-Transfer.git
cd File-Transfer
```

The project includes a build script that can be used to compile the application:

```bash
chmod +x Build.sh
./Build.sh
```

The `Build.sh` script handles the compilation process and creates the executable.

## Usage

Start the receiver on the destination machine:

```bash
./file-transfer
```

Then start the sender and provide the required destination information and file path.

The exact command-line interface depends on the available transfer options.

## Transfer Process

A file is opened in binary mode and read in chunks into a buffer:

```cpp
ifstream source_file(path, ios::binary);
```

The data is then transmitted through a TCP socket:

```cpp
send(socket_fd, buffer, bytes_read, 0);
```

On the receiving side, the data is read from the socket:

```cpp
recv(socket_fd, buffer, sizeof(buffer), 0);
```

and written to the destination file.

This approach allows files larger than the socket buffer to be transferred without loading the entire file into memory.

## Project Structure

```text
File-Transfer/
├── Build.sh
├── main.cpp
├── ...
├── LICENSE
└── README.md
```

## Planned Improvements

* [ ] Transfer metadata
* [ ] File size verification
* [ ] Transfer completion detection
* [ ] Better error handling
* [ ] Connection timeouts
* [ ] Transfer progress
* [ ] File integrity verification
* [ ] Resume interrupted transfers
* [ ] Improved command-line interface
* [ ] Cleaner client/server architecture
* [ ] More robust SSH integration

## Security

SSH is used for remote access and process startup where required.

The application itself is a learning project and should **not be considered production-ready or a secure replacement for established file-transfer software** such as SCP or SFTP.

Do not use it to transfer sensitive data over untrusted networks without understanding the security properties of the current implementation.

## Why This Project?

This project is primarily a practical exercise in **Linux network programming and systems programming**.

Instead of relying on a high-level file-transfer library, the implementation focuses on understanding what happens underneath:

```text
Application
    ↓
File I/O
    ↓
Buffer
    ↓
TCP Socket
    ↓
Network
    ↓
TCP Socket
    ↓
Buffer
    ↓
File I/O
```

The goal is to understand the networking and operating-system mechanisms involved in file transmission rather than simply implementing another file-transfer application.

## License

This project is licensed under the **MIT License**.

## Author

**Kaveh Nazem**
