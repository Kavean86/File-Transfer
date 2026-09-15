#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "socket_utils.h"
using namespace std;


// Start the receiver server and handle the file transfer.
void receive()
{
    // Create a TCP socket.
    int recv_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if (recv_socket < 0)
    {
        perror("socket");
        return;
    }


    // Configure the server address.
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8756);
    addr.sin_addr.s_addr = INADDR_ANY;


    // Bind the socket to port 8756 on all network interfaces.
    if (bind(recv_socket, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");

        close(recv_socket);

        return;
    }


    // Put the socket into listening mode.
    if (listen(recv_socket, 1) < 0)
    {
        perror("listen");

        close(recv_socket);

        return;
    }


    // Store information about the connecting client.
    sockaddr_in clientaddr{};

    socklen_t client_size =
        sizeof(clientaddr);


    // Accept a connection from a client.
    int client = accept(recv_socket, (sockaddr*)&clientaddr, &client_size);

    if (client < 0)
    {
        perror("accept");

        close(recv_socket);

        return;
    }


    // Acknowledgement message used during the transfer handshake.
    string ACK = "ACK";

    // Store the transfer type and destination/source path.
    string type;
    string path;

    // Receive the transfer type from the client.
    if (!recv_line(client, type))
    {
        cerr << "Failed to receive type\n";

        close(client);
        close(recv_socket);

        return;
    }

    // Confirm that the transfer type was received.
    if (!send_all(client, ACK.c_str(), ACK.size()))
    {
        close(client);
        close(recv_socket);

        return;
    }

    // Receive the file path from the client.
    if (!recv_line(client, path))
    {
        cerr << "Failed to receive destination path\n";

        close(client);
        close(recv_socket);

        return;
    }

    // Confirm that the path was received.
    if (!send_all(client, ACK.c_str(), ACK.size()))
    {
        close(client);
        close(recv_socket);

        return;
    }

//    cout<<type<<endl<<path;

    // Handle receiving a file from the client.
    if (type == "-s")
    {
        // Open the destination file in binary mode.
        ofstream output(path, ios::binary);


        if (!output)
        {
            perror("open");

            close(client);
            close(recv_socket);

            return;
        }


        // Buffer used to receive file data.
        char buffer[1024];


        // Receive file chunks until the client signals the end.
        while (true)
        {
            // Receive the size of the next data block.
            string len;

            if (!recv_line(client, len))
            {

                break;
            }


            int bytes_to_receive;


            // Convert the received length from string to integer.
            try
            {
                bytes_to_receive =
                    stoi(len);
            }
            catch (...)
            {
                cerr << "Invalid length: "
                     << len
                     << '\n';

                break;
            }

            // Confirm that the length was received.
            if (!send_all(client, ACK.c_str(), ACK.size()))
            {
                break;
            }


            // A length of zero indicates the end of the file.
            if (bytes_to_receive == 0)
            {
                break;
            }


            int total_received = 0;

            // Continue receiving until the complete block is received.
            while (total_received < bytes_to_receive)
            {
                int remaining =
                    bytes_to_receive -
                    total_received;


                // Determine how much data to receive in this iteration.
                int chunk_size =
                    remaining;


                // Limit each recv() operation to the buffer size.
                if (chunk_size > 1024)
                {
                    chunk_size = 1024;
                }


                // Receive a portion of the file.
                ssize_t received =
                    recv(client, buffer, chunk_size, 0);


                if (received <= 0)
                {
                    perror("recv");

                    output.close();

                    close(client);
                    close(recv_socket);

                    return;
                }


                // Write the received data to the output file.
                output.write(buffer, received);


                // Update the number of bytes received for this block.
                total_received += received;
            }
        }


        // Close the output file after receiving is complete.
        output.close();

        cout << "File received successfully\n";
    }

    // Handle sending a file to the client.
    if(type=="-r"){
    
    // Open the source file in binary mode.
    ifstream source_file(path,ios::binary);
    char buffer[1024];

    // Read the file in 1024-byte chunks and send each chunk.
    while (source_file.read(buffer, sizeof(buffer)) || source_file.gcount() > 0)
    {
        streamsize bytes_read = source_file.gcount();

	send_all(client, buffer, bytes_read);
    }

    }

    // Close the client and server sockets.
    close(client);
    close(recv_socket);
}

