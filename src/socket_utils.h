#pragma once

#include <sys/socket.h>
#include <string>
using namespace std;


// Send exactly "length" bytes through the socket.
bool send_all(int socket, const char* data, size_t length)
{
    // Keep track of the total number of bytes sent.
    size_t total = 0;

    // Continue sending until all requested bytes have been transmitted.
    while (total < length)
    {
        ssize_t sent = send(socket,data + total,length - total,0);

        // Return false if sending fails or the connection is closed.
        if (sent <= 0)
        {
            return false;
        }

        // Update the number of bytes successfully sent.
        total += sent;
    }

    return true;
}


// Receive exactly "length" bytes from the socket.
bool recv_all(int socket, char* data, size_t length)
{
    // Keep track of the total number of bytes received.
    size_t total = 0;

    // Continue receiving until all requested bytes have been received.
    while (total < length)
    {
        ssize_t received = recv(socket,data + total,length - total,0);

        // Return false if receiving fails or the connection is closed.
        if (received <= 0)
        {
            return false;
        }

        // Update the number of bytes successfully received.
        total += received;
    }

    return true;
}


// Receive data from the socket until a newline character is found.
bool recv_line(int socket, string& data)
{
    // Clear any previous contents of the string.
    data.clear();

    // Store one received character at a time.
    char c;

    while (true)
    {
        // Receive a single character from the socket.
        ssize_t received = recv(socket,&c,1,0);

        // Return false if receiving fails or the connection is closed.
        if (received <= 0)
        {
            return false;
        }

        // Stop reading when the newline delimiter is reached.
        if (c == '\n')
        {
            break;
        }

        // Append the received character to the output string.
        data += c;
    }

    return true;
}
