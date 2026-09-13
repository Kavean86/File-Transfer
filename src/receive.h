#pragma once
#include <fstream>
#include <cstring>
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
string ACK="ACK";

string meta[3];

for (int i = 0; i < 3; i++) {
ssize_t meta_received = recv(client, buffer, sizeof(buffer), 0);
if (meta_received <= 0)
break;
meta[i] = string(buffer, meta_received);
send(client, ACK.c_str(), ACK.size(), 0);
memset(buffer, 0, sizeof(buffer));
}
string type=meta[0];
string path=meta[1];
string len=meta[2];

if(type=="-s"){
ssize_t received = recv(client, buffer, sizeof(buffer), 0);

string data(buffer, received);
//send(client, ACK.c_str(), ACK.size(), 0);
memset(buffer, 0, sizeof(buffer));

cout<<type<<endl<<path<<endl<<len<<endl<<endl<<data;
}



}
