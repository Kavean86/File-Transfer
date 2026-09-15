#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include "ssh.h"
#include "socket_utils.h"

using namespace std;


// Send a file to the remote system.
void send_file(
    string source_path,
    string dest_path,
    string dest_ip,
    string type
)
{
    // Start the SSH process on the destination system.
    pid_t pid = start_ssh(dest_ip);

    if (pid < 0)
    {
        return;
    }


    // Create a TCP socket for the file transfer.
    int send_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if (send_socket < 0)
    {
        perror("socket");

        // Terminate the SSH process if socket creation fails.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    // Configure the destination server address.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8756);


    // Convert the destination IP address from text to binary form.
    if (inet_pton(AF_INET,dest_ip.c_str(),&addr.sin_addr) <= 0)
    {
        cerr << "Invalid IP address\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    // Keep trying to connect until the remote receiver is ready.
    while (true)
    {
        if (connect(send_socket,reinterpret_cast<sockaddr*>(&addr),sizeof(addr)) == 0)
        {
            break;
        }

        // Wait briefly before trying again.
        usleep(200000);
    }


    // Buffer for receiving the 3-byte ACK messages.
    char ack[3];

    // Send the transfer type to the receiver.
    string type_data = type + "\n";

    if (!send_all(send_socket,type_data.c_str(),type_data.size())){
        perror("send type");
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Wait for the receiver to acknowledge the transfer type.
    if (!recv_all(send_socket,ack,3)){
        cerr << "Receiver did not ACK type\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Clear the ACK buffer before receiving the next ACK.
    memset(ack, 0, sizeof(ack));

    // Send the destination path to the receiver.
    string path_data = dest_path + "\n";

    if (!send_all(send_socket,path_data.c_str(),path_data.size())){
        perror("send destination path");
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Wait for the receiver to acknowledge the destination path.
    if (!recv_all(send_socket,ack,3))
    {
        cerr << "Receiver did not ACK destination path\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Clear the ACK buffer after receiving the path acknowledgement.
    memset(ack, 0, sizeof(ack));

    // Open the source file in binary mode.
    ifstream source_file(source_path,ios::binary);

    if (!source_file.is_open()){
        cerr << "Can not open file\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    // Buffer used to read file data before sending.
    char buffer[1024];

    // Read and send the file in chunks.
    while (true)
    {
        // Read up to 1024 bytes from the source file.
        source_file.read(buffer,sizeof(buffer));

        // Get the actual number of bytes read.
        streamsize bytes_read =source_file.gcount();

        // Convert the number of bytes into a newline-terminated string.
        string bytes =to_string(bytes_read) + "\n";

        // Send the size of the next data block.
        if (!send_all(send_socket,bytes.c_str(),bytes.size())){
            perror("send bytes");
            break;
        }

        // Wait for the receiver to acknowledge the block size.
        if (!recv_all(send_socket,ack,3)){
            cerr << "Receiver closed connection\n";
            break;
        }

        // Clear the ACK buffer.
        memset(ack, 0, sizeof(ack));

        // Send the actual file data if bytes were read.
        if (bytes_read > 0)
        {
            if (!send_all(send_socket,buffer,bytes_read)){
                perror("send file data");
                break;
            }
        }

        // Stop after the final block has been processed.
        if (source_file.eof()){
            break;
        }
    }


    // Close the source file.
    source_file.close();

    // Close the TCP connection.
    close(send_socket);

    // Wait for the SSH process to finish.
    waitpid(pid, nullptr, 0);
}
