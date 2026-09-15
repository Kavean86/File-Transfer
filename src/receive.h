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


void receive()
{
    int recv_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if (recv_socket < 0)
    {
        perror("socket");
        return;
    }


    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8756);
    addr.sin_addr.s_addr = INADDR_ANY;


    if (bind(recv_socket, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");

        close(recv_socket);

        return;
    }


    if (listen(recv_socket, 1) < 0)
    {
        perror("listen");

        close(recv_socket);

        return;
    }


    sockaddr_in clientaddr{};

    socklen_t client_size =
        sizeof(clientaddr);


    int client = accept(recv_socket, (sockaddr*)&clientaddr, &client_size);

    if (client < 0)
    {
        perror("accept");

        close(recv_socket);

        return;
    }


    string ACK = "ACK";

    string type;
    string path;

    if (!recv_line(client, type))
    {
        cerr << "Failed to receive type\n";

        close(client);
        close(recv_socket);

        return;
    }

    if (!send_all(client, ACK.c_str(), ACK.size()))
    {
        close(client);
        close(recv_socket);

        return;
    }

    if (!recv_line(client, path))
    {
        cerr << "Failed to receive destination path\n";

        close(client);
        close(recv_socket);

        return;
    }

    if (!send_all(client, ACK.c_str(), ACK.size()))
    {
        close(client);
        close(recv_socket);

        return;
    }

    if (type == "-s")
    {
        ofstream output(path, ios::binary);


        if (!output)
        {
            perror("open");

            close(client);
            close(recv_socket);

            return;
        }


        char buffer[1024];


        while (true)
        {
            string len;

            if (!recv_line(client, len))
            {

                break;
            }


            int bytes_to_receive;


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

            if (!send_all(client, ACK.c_str(), ACK.size()))
            {
                break;
            }


            if (bytes_to_receive == 0)
            {
                break;
            }


            int total_received = 0;

            while (total_received < bytes_to_receive)
            {
                int remaining =
                    bytes_to_receive -
                    total_received;


                int chunk_size =
                    remaining;


                if (chunk_size > 1024)
                {
                    chunk_size = 1024;
                }


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


                output.write(buffer, received);


                total_received += received;
            }
        }


        output.close();

        cout << "File received successfully\n";
    }


    close(client);
    close(recv_socket);
}


