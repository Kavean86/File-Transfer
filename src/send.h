#pragma once
#include <fstream>
using namespace std;

void send(string source_path , string dest_path){
//system("ssh 185.164.72.175");

int send_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

if(send_socket<0){
perror("socket");
return;
}

}
