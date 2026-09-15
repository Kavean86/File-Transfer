#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

pid_t start_ssh(string dest_ip)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        string host = "root@" + dest_ip;

        execlp("ssh","ssh",host.c_str(),"/root/a.out","-l",(char*)nullptr);

        perror("execlp");
        _exit(1);
    }

    return pid;
}
