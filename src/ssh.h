#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>

using namespace std;


// Start the remote receiver program through SSH.
pid_t start_ssh(string dest_ip)
{
    // Create a child process.
    pid_t pid = fork();

    if (pid < 0)
    {
        // Report an error if creating the child process fails.
        perror("fork");
        return -1;
    }

    // Code executed by the child process.
    if (pid == 0)
    {
        // Build the SSH target in the form root@IP.
        string host = "root@" + dest_ip;

        // Replace the child process with an SSH process
        // and execute the remote /root/a.out program.
        execlp("ssh","ssh",host.c_str(),"/root/a.out","-l",(char*)nullptr);

        // This point is reached only if execlp() fails.
        perror("execlp");
        _exit(1);
    }

    // Return the PID of the SSH child process to the parent.
    return pid;
}
