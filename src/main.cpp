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
 
// Check if at least one argument was provided.
if(argc < 2){ 
cerr<<"invalid arguments\n"; 
return 1; 
} 
 
// Store the first command-line argument as the operation type.
string type=argv[1]; 
 
// If the type is "-l", start the receiver/listener.
if(type=="-l"){ 
receive(); 
return 0; 
} 
 
// Send and receive operations require at least 4 additional arguments.
if(argc < 5){ 
cerr<<"invalid arguments\n"; 
return 1; 
} 
 
// Store the source path, destination path, and target IP address.
string path1=argv[2]; 
string path2=argv[3]; 
string IP=argv[4]; 
 
// Accept only "-s" (send) or "-r" (receive) as the operation type.
regex type_regex(R"(^-[sr]$)"); 
 
// Validate the provided file paths.
regex path_regex(R"(^(\/|\.{1,2}\/|[a-zA-Z0-9_.-]+\/)[a-zA-Z0-9_.\/-]*$)"); 
 
// Reject an invalid operation type.
if(!regex_match(type,type_regex)){ 
cerr<<"invalid type\n"; 
return 1; 
} 
 
// Reject invalid source or destination paths.
if(!regex_match(path1,path_regex) || !regex_match(path2,path_regex)){ 
cerr<<"invalid path\n"; 
return 1; 
} 
 
// Send a file from the local system to the remote system.
if(type=="-s"){ 
 
send_file(path1,path2,IP,type); 
} 
 
// Receive a file from the remote system to the local system.
if(type=="-r"){ 
 
recv_file(IP,type,path2,path1); 
} 
 
}
