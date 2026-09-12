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

using namespace std;

void send_file(string source_path, string dest_path, string dest_ip)
{
 
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {

        string host = dest_ip;

        execlp("ssh","ssh",host.c_str(),"nc -l 8756",(char*)nullptr);
        perror("execlp");
        _exit(1);
    }

    int send_socket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (send_socket < 0) {
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

    while (true) {

        if (connect(send_socket,reinterpret_cast<sockaddr*>(&addr),sizeof(addr)) == 0)
        {
            cout << "Connected to mycp-server\n";
            break;
        }

        usleep(200000);
    }

    string test = "12765kaveh";

    ssize_t sent = ::send(send_socket,test.c_str(),test.size(),0);

    if (sent < 0) {
        perror("send");
    }

    close(send_socket);

    waitpid(pid, nullptr, 0);
}
