//#include<iostream>
//#include<fstream>
//#include<random>
//#include<functional>
//#include<bitset>
//#include<string>
//#include"./include/routing_table.h"
//#include"./include/id.h"
//#include <kademlia/message.h>
//#include <networking.h>
//#include <thread>
#include "./include/interface.h"

//uint16_t PORT;

#include <boost/asio.hpp>

int main(int argc, char* argv[]){

	//std::string id{"23479328"};
	//kademlia::network::client client{PORT,id};
	//std::thread receive_thread([&] {client.receive();});
	//receive_thread.join();

  //event_loop();

  boost::asio::socket sock{"hello"};



	return 0;
}
