#include<iostream>
#include<fstream>
#include<random>
#include<functional>
#include<bitset>
#include<string>
#include"./include/routing_table.h"
#include"./include/id.h"
#include <kademlia/message.h>
#include <networking.h>
#include <thread>

uint16_t PORT;

int main(int argc, char* argv[]){

	std::string id{"23479328"};
	kademlia::network::client client{PORT,id};

	std::thread receive_thread([&] {client.receive();});
	
	receive_thread.join();

	return 0;
}
