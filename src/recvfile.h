#include <fstream>
using namespace std;

void recv_file(string dest_ip,string type,string source_path,string dest_path){

pid_t pid = start_ssh(dest_ip);

if (pid < 0) {
	    return;
}
int send_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if (send_socket < 0)
    {
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


    while (true)
    {
        if (connect(send_socket,reinterpret_cast<sockaddr*>(&addr),sizeof(addr)) == 0)
        {
            break;
        }

        usleep(200000);
    }


char ack[3];

    string type_data = type + "\n";

    if (!send_all(send_socket,type_data.c_str(),type_data.size())){
        perror("send type");
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    if (!recv_all(send_socket,ack,3)){
        cerr << "Receiver did not ACK type\n";
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    memset(ack, 0, sizeof(ack));
    string path_data = dest_path + "\n";

    if (!send_all(send_socket,path_data.c_str(),path_data.size())){
        perror("send destination path");
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    if (!recv_all(send_socket,ack,3))
    {
        cerr << "Receiver did not ACK destination path\n";
        close(send_socket);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    memset(ack, 0, sizeof(ack));

ofstream output(source_path, ios::binary);

if (!output)
{
perror("open");
return;
}

char buffer[1024];

while (true)
{
ssize_t received = recv(send_socket, buffer, sizeof(buffer), 0);
if (received <= 0){
break;
}
output.write(buffer, received);
}

cout << "File received successfully\n";
output.close();

waitpid(pid, nullptr, 0);
}
