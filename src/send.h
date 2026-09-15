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


void send_file(
    string source_path,
    string dest_path,
    string dest_ip,
    string type
)
{
    pid_t pid = start_ssh(dest_ip);

    if (pid < 0)
    {
        return;
    }


    int send_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if (send_socket < 0)
    {
        perror("socket");
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8756);


    if (inet_pton(AF_INET,dest_ip.c_str(),&addr.sin_addr) <= 0)
    {
        cerr << "Invalid IP address\n";
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    while (true)
    {
        if (connect(send_socket,reinterpret_cast<sockaddr*>(&addr),sizeof(addr)) == 0)
        {
            break;
        }

        usleep(200000);
    }


    char ack[3];

    string type_data = type + "\n";

    if (!send_all(send_socket,type_data.c_str(),type_data.size())){
        perror("send type");
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    if (!recv_all(send_socket,ack,3)){
        cerr << "Receiver did not ACK type\n";
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    memset(ack, 0, sizeof(ack));
    string path_data = dest_path + "\n";

    if (!send_all(send_socket,path_data.c_str(),path_data.size())){
        perror("send destination path");
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    if (!recv_all(send_socket,ack,3))
    {
        cerr << "Receiver did not ACK destination path\n";
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    memset(ack, 0, sizeof(ack));

    ifstream source_file(source_path,ios::binary);

    if (!source_file.is_open()){
        cerr << "Can not open file\n";
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    char buffer[1024];

    while (true)
    {
        source_file.read(buffer,sizeof(buffer));

        streamsize bytes_read =source_file.gcount();
        string bytes =to_string(bytes_read) + "\n";

        if (!send_all(send_socket,bytes.c_str(),bytes.size())){
            perror("send bytes");
            break;
        }

        if (!recv_all(send_socket,ack,3)){
            cerr << "Receiver closed connection\n";
            break;
        }

        memset(ack, 0, sizeof(ack));

        if (bytes_read > 0)
        {
            if (!send_all(send_socket,buffer,bytes_read)){
                perror("send file data");
                break;
            }
        }

        if (source_file.eof()){
            break;
        }
    }


    source_file.close();

    close(send_socket);

    waitpid(pid, nullptr, 0);
}
