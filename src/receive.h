#pragma once
#include <fstream>
using namespace std;

void receive(){
int recv_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(recv_socket<0){
perror("socket");
return;
}
sockaddr_in addr{};
addr.sin_family=AF_INET;
addr.sin_port=htons(8756);
addr.sin_addr.s_addr=INADDR_ANY;
if(bind(recv_socket,(sockaddr*)&addr,sizeof(addr))<0){
perror("bind");
return;
}
if(listen(recv_socket,1)<0){
perror("listen");
return;
}

sockaddr_in clientaddr;
socklen_t client_size=sizeof(clientaddr);
int client=accept(recv_socket,(sockaddr*)&clientaddr,&client_size);
if(client<0){
perror("accept");
return;
}
char buffer[1024];
recv(recv_socket,buffer,sizeof(buffer),0);
cout<<buffer;
}
