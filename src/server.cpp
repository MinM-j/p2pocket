#include<iostream>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>

constexpr uint16_t Port = 3478;
int serverSocket;

constexpr int BUFLEN = 1024;
char buffer[BUFLEN];

std::map<int,std::pair<std::string, struct sockaddr_in>> current;

void sendResponse(const std::string& sender, sockaddr_in receiver){
	sendto(serverSocket,sender.c_str(),sender.length(),0,(sockaddr*)&receiver,sizeof(receiver));
}

int main(){
	if((serverSocket = socket(AF_INET,SOCK_DGRAM,0)) < 0){
		std::cout << "Socket failed";
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(Port);
	server_address.sin_addr.s_addr = INADDR_ANY;
	if(bind(serverSocket,(sockaddr*)&server_address,sizeof(server_address)) < 0){
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
	int val = 64 * 1024;
	setsockopt(serverSocket,SOL_SOCKET,SO_SNDBUF,(char*)&val, sizeof(val));
	setsockopt(serverSocket,SOL_SOCKET,SO_RCVBUF,(char*)&val, sizeof(val));
	listen(serverSocket,1000);
	while (true) {
		sockaddr_in client_address;
		socklen_t client_size = sizeof(client_address);
		int result = recvfrom(serverSocket,buffer,BUFLEN,0,(sockaddr*)&client_address,&client_size);
		if(result > 0){
			std::string message = std::string(buffer,buffer+result);
			int id = stoi(message.substr(0,message.find(" ")));
			std::string public_presence = message.substr(message.find(" ")+1);
			if(current.find(id) != current.end()){
				std::pair<std::string, struct sockaddr_in> other = current[id];
				sendResponse(other.first,client_address);
				sendResponse(public_presence,other.second);
				std::cout << "\nLinked\nID : " << id << "\nEndpoint1 : " << other.first << "\nEndpoint2 : " << public_presence << std::endl;
				current.erase(id);
			}
			else{
				printf("%s",message.c_str());
				current.insert(std::make_pair(id,std::make_pair(public_presence,client_address)));
				printf("\nRegistered\nID : %d\nEndpoint : %s",id, public_presence.c_str());
			}
		}
		sleep(1);
	}
	close(serverSocket);
	return 0;
}
