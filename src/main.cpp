#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <regex>
#include "send.h"
#include "receive.h"
#include "ssh.h"
#include "recvfile.h"
using namespace std;

int main(int argc,char* argv[]){

if(argc < 2){
cerr<<"invalid arguments\n";
return 1;
}

string type=argv[1];

if(type=="-l"){
receive();
return 0;
}

if(argc < 5){
cerr<<"invalid arguments\n";
return 1;
}

string path1=argv[2];
string path2=argv[3];
string IP=argv[4];

regex type_regex(R"(^-[sr]$)");
regex path_regex(R"(^(\/|\.{1,2}\/|[a-zA-Z0-9_.-]+\/)[a-zA-Z0-9_.\/-]*$)");

if(!regex_match(type,type_regex)){
cerr<<"invalid type\n";
return 1;
}

if(!regex_match(path1,path_regex) || !regex_match(path2,path_regex)){
cerr<<"invalid path\n";
return 1;
}

if(type=="-s"){

send_file(path1,path2,IP,type);
}
if(type=="-r"){
//start_ssh(IP);
recv_file(IP);
}

}
