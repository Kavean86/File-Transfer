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
using namespace std;

void send_file(string source_path, string dest_path, string dest_ip , string type)
{
 
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {

        string host = "root@"+dest_ip;

        execlp("ssh","ssh",host.c_str(),"/root/a.out -r",(char*)nullptr);
        perror("execlp");
        _exit(1);
    }

    int send_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

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
            break;
        }

        usleep(200000);
    }
//
  //  
    string newline="\n";
    char ack[3];

    send(send_socket,type.c_str(),type.size(),0);
    if(recv(send_socket,ack,sizeof(ack),0)>0){
	    memset(ack,0,sizeof(ack));
	    send(send_socket,dest_path.c_str(),dest_path.size(),0);
	    if(recv(send_socket,ack,sizeof(ack),0)>0){
	    memset(ack,0,sizeof(ack));
	    }
    }

    char buffer[1024];
    ifstream source_file(source_path,ios::binary);

    if(!source_file.is_open()){
    cout<<"can not open file\n";
    }

    while (true) {

    source_file.read(buffer, sizeof(buffer));

    streamsize bytes_read = source_file.gcount();
    string bytes=to_string(bytes_read);

   send(send_socket,bytes.c_str(),bytes.size(),0);
   if(recv(send_socket,ack,sizeof(ack),0)>0){
       memset(ack,0,sizeof(ack));
       send(send_socket,buffer,bytes_read,0);
   }

     if (source_file.eof()) {
        break;
    }
     
    }
    
    source_file.close();
    close(send_socket);

    waitpid(pid, nullptr, 0);
}
