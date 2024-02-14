#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <string>
#include <cstring>
#include <thread>

constexpr uint16_t bindingRequest = 0x0001;
constexpr uint16_t bindingResponse = 0x0101;
constexpr uint16_t XORMappedAddress = 0x0020;
constexpr uint32_t magicCookie = 0x2112A442;

const std::string stunServer = "stun2.l.google.com";
constexpr uint16_t stunServerPort = 19305;

bool local = false;
const std::string serverIP = "147.185.221.16";
constexpr uint16_t serverPort = 15219;

const std::string localServerIP = "127.0.0.1";
constexpr uint16_t localServerPort = 3478;

constexpr int BUFLEN = 1024;
char buffer[BUFLEN];

int own_socket;

struct __attribute__((packed)) stunRequest{
	stunRequest(){
		for (int i = 0; i < transaction_id.size(); i++) {
			transaction_id[i] = i % 256;
		}
	}
	const uint16_t stun_message_type = htons(bindingRequest);
	const uint16_t message_length = htons(0);
	const uint32_t magic_cookie = htonl(magicCookie);
	std::array<uint8_t, 12> transaction_id;
};

struct __attribute__((packed)) stunResponse{
	uint16_t stun_message_type;
	uint16_t message_length;
	uint32_t magic_cookie;
	std::array<uint8_t, 12> transaction_id;
	std::array<uint8_t, 1000> attributes;
};

struct IPAndPort{
	std::string ip;
	uint16_t port;
};

void error(const std::string& error){
	perror(error.c_str());
	exit(EXIT_FAILURE);
}

std::string resolveDomain(std::string domain){
	char ip[50];
	hostent* host;
	in_addr** address_list;
	if ((host = gethostbyname(domain.c_str())) == NULL)
		error("Error in resolving domain");
	address_list = (in_addr **)host->h_addr_list;
	for (int i = 0; address_list[i] != NULL; i++) {
		strcpy(ip,inet_ntoa(*address_list[i]));
	}
	return std::string(ip);
}

std::string stunRequestResponse (int &own_socket){
	struct IPAndPort own_ip_port;
	struct stunRequest request;
	struct stunResponse response;
	struct sockaddr_in server_addr;
	std::string server_ip = resolveDomain(stunServer);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(stunServerPort);
	inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
	if (sendto(own_socket, &request, sizeof(request), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
		error("Error in sendto");
	if (recvfrom(own_socket, &response, sizeof(response), 0, NULL, 0) < 0)
		error("Error in recvfrom");
	if (response.magic_cookie != htonl(magicCookie))
		error("Magic cookie	mismatch");
	if (response.transaction_id != request.transaction_id)
		error("Incorrect transaction_id");
	if (response.stun_message_type != htons(bindingResponse))
		error("Incorrect message type");	
	const auto& attributes = response.attributes;
	int16_t attributes_length = std::min<int16_t>(htons(response.message_length),response.attributes.size());
	int i{};
	while (i < attributes_length){
		auto attribute_type = htons(*(int16_t*)&attributes[i]);
		auto attribute_length = htons(*(int16_t*)&attributes[i+2]);
		if (attribute_type == XORMappedAddress) {
			uint16_t port = ntohs(*(uint16_t*)&attributes[i+6]);
			port ^= (magicCookie >> 16);
			std::string ip = std::to_string(attributes[i+8] ^ ((magicCookie & 0xff000000) >> 24)) + "." +
							 std::to_string(attributes[i+9] ^ ((magicCookie & 0x00ff0000) >> 16)) + "."	+
							 std::to_string(attributes[i+10] ^ ((magicCookie & 0x0000ff00) >> 8)) + "."	+
							 std::to_string(attributes[i+11] ^ ((magicCookie & 0x000000ff) >> 0));
			own_ip_port.ip = ip;
			own_ip_port.port = port;
			return ip + ":" + std::to_string(own_ip_port.port);
		}
		i += (4 + attribute_length);
	}
	return "error";
}

void receiving(){
	while (true) {
		int result = recvfrom(own_socket, buffer, BUFLEN, 0, NULL, 0);
    std::cout << "Received return value"<< result<<std::endl;
		if (result > 0){
			std::string message = std::string(buffer, buffer+result);
			printf("%s\n",message.c_str());
		}
		else{
			printf("Error : Peer closed\n");
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char** argv){
	struct sockaddr_in own_address;
	own_socket = socket(AF_INET, SOCK_DGRAM, 0);
	own_address.sin_family = AF_INET;
	own_address.sin_port = htons(2703);
	std::string condition = "123";
	if (argc > 1 && strcmp(argv[1],condition.c_str()) == 0)
		own_address.sin_port = htons(1208);
	else
		own_address.sin_port = htons(2703);
	if (bind(own_socket, (struct sockaddr *)&own_address, sizeof(own_address)) < 0)
		error("Binding error");
	std::string own_public_presence = stunRequestResponse(own_socket);
	printf("My public presence : %s\n",own_public_presence.c_str());
	struct sockaddr_in server_address;	server_address.sin_family = AF_INET;
	if (local) {
		server_address.sin_port = htons(localServerPort);
		inet_pton(AF_INET, localServerIP.c_str(), &server_address.sin_addr);
	}
	else{
		server_address.sin_port = htons(serverPort);
		inet_pton(AF_INET, serverIP.c_str(), &server_address.sin_addr);
	}
	int val = 64 * 1024;
	setsockopt(own_socket, SOL_SOCKET, SO_SNDBUF, (char *)&val,sizeof(val));
	setsockopt(own_socket, SOL_SOCKET, SO_RCVBUF, (char *)&val, sizeof(val));
	std::string indentification;
	printf("\nEnter indentification : ");
	std::cin >> indentification;
	std::string message = indentification + " " + own_public_presence;

	printf("%s",message.c_str());
	sleep(1);
	if (sendto(own_socket, message.c_str(), message.length(), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) 
		error("Error in sendto");
	bool not_found{true};
	std::string endpoint;
	struct sockaddr_in other_address;

	while (not_found) {
		int result = recvfrom(own_socket, buffer, BUFLEN, 0, NULL, 0);
		if (result > 0) {
			endpoint = std::string(buffer,buffer+result);
			printf("\nConnected Endpoint: %s\n", endpoint.c_str());

			std::string endpoint_ip = own_public_presence.substr(0,endpoint.find(":"));
			uint16_t endpoint_port = stoi(own_public_presence.substr(endpoint.find(":")+1));

			//std::string endpoint_ip = endpoint.substr(0,endpoint.find(":"));
			//uint16_t endpoint_port = stoi(endpoint.substr(endpoint.find(":")+1));
			printf("\nEndpoint ip : %s\n Endpoint port : %d\n",endpoint_ip.c_str(),endpoint_port);
			other_address.sin_family = AF_INET;
      other_address.sin_port = htons(endpoint_port);
			//other_address.sin_port = htons(2703);
			inet_pton(AF_INET, endpoint_ip.c_str(), &other_address.sin_addr);
			int temp = sendto(own_socket, "HI", 2, 0, (struct sockaddr *)&other_address, sizeof(other_address));
      std::cout<< temp<<std::endl;
			not_found = false;
		}
	}
	std::thread receive{receiving};
	int i{1};
	while(true){
		message = "Hello " + std::to_string(i);
    int status = 	sendto(own_socket, message.c_str(), message.length(), 0, (struct sockaddr *)&other_address, sizeof(other_address));
		i++;
		sleep(1);
	}
	close(own_socket);
	return 0;
}
