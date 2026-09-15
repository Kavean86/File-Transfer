#include <fstream>
using namespace std;

// Receive a file from the remote system.
void recv_file(string dest_ip,string type,string source_path,string dest_path){

// Start the SSH process on the destination system.
pid_t pid = start_ssh(dest_ip);

if (pid < 0) {
	    return;
}

// Create a TCP socket for the file transfer.
int send_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if (send_socket < 0)
    {
        perror("socket");

        // Terminate the SSH process if socket creation fails.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    // Configure the destination server address.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8756);


    // Convert the destination IP address from text to binary form.
    if (inet_pton(AF_INET,dest_ip.c_str(),&addr.sin_addr) <= 0)
    {
        cerr << "Invalid IP address\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }


    // Keep trying to connect until the remote receiver is ready.
    while (true)
    {
        if (connect(send_socket,reinterpret_cast<sockaddr*>(&addr),sizeof(addr)) == 0)
        {
            break;
        }

        // Wait briefly before trying again.
        usleep(200000);
    }


// Buffer for receiving the 3-byte ACK message.
char ack[3];

    // Send the transfer type to the receiver.
    string type_data = type + "\n";

    if (!send_all(send_socket,type_data.c_str(),type_data.size())){
        perror("send type");
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Wait for the receiver to acknowledge the transfer type.
    if (!recv_all(send_socket,ack,3)){
        cerr << "Receiver did not ACK type\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Clear the ACK buffer before receiving the next ACK.
    memset(ack, 0, sizeof(ack));

    // Send the destination path to the receiver.
    string path_data = dest_path + "\n";

    if (!send_all(send_socket,path_data.c_str(),path_data.size())){
        perror("send destination path");
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Wait for the receiver to acknowledge the destination path.
    if (!recv_all(send_socket,ack,3))
    {
        cerr << "Receiver did not ACK destination path\n";
        close(send_socket);

        // Clean up the SSH process.
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        return;
    }

    // Clear the ACK buffer after receiving the second ACK.
    memset(ack, 0, sizeof(ack));

// Open the local destination file in binary mode.
ofstream output(source_path, ios::binary);

if (!output)
{
perror("open");
return;
}

// Buffer used to receive file data.
char buffer[1024];

// Receive file data until the sender closes the connection.
while (true)
{
ssize_t received = recv(send_socket, buffer, sizeof(buffer), 0);
if (received <= 0){
break;
}

// Write the received data to the local file.
output.write(buffer, received);
}

// Indicate that the file transfer has completed.
cout << "File received successfully\n";

output.close();

// Wait for the SSH process to finish.
waitpid(pid, nullptr, 0);
}
