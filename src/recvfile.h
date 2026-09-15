#include <fstream>
using namespace std;

void recv_file(string dest_ip){
cout<<"user -r\n";
pid_t pid = start_ssh(dest_ip);

if (pid < 0) {
	    return;
}
waitpid(pid, nullptr, 0);
}
