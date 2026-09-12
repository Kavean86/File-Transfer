#pragma once
#include <fstream>
using namespace std;

void send(string source_path , string dest_path , string dest_ip){
//system("ssh 185.164.72.175");

int send_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

if(send_socket<0){
perror("socket");
return;
}
sockaddr_in addr;
addr.sin_family=AF_INET;
addr.sin_port=htons(8756);
inet_pton(AF_INET,dest_ip.c_str(),&addr.sin_addr);

connect(send_socket,(sockaddr*)&addr,sizeof(addr));
}
