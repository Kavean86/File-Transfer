#pragma once

#include <sys/socket.h>
#include <string>
using namespace std;

bool send_all(int socket, const char* data, size_t length)
{
    size_t total = 0;

    while (total < length)
    {
        ssize_t sent = send(socket,data + total,length - total,0);

        if (sent <= 0)
        {
            return false;
        }

        total += sent;
    }

    return true;
}


bool recv_all(int socket, char* data, size_t length)
{
    size_t total = 0;

    while (total < length)
    {
        ssize_t received = recv(socket,data + total,length - total,0);

        if (received <= 0)
        {
            return false;
        }

        total += received;
    }

    return true;
}


bool recv_line(int socket, string& data)
{
    data.clear();

    char c;

    while (true)
    {
        ssize_t received = recv(socket,&c,1,0);

        if (received <= 0)
        {
            return false;
        }

        if (c == '\n')
        {
            break;
        }

        data += c;
    }

    return true;
}
