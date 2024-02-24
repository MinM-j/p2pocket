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
#include"./include/storage/sha1.h"
#include"./include/storage/storage.h"

uint16_t PORT;

int main(int argc, char* argv[]){
  //std::cout<<generate_id()<<std::endl;

	/*

	RoutingTable rt{"101100"};


	rt.print();
	ID id{"101101"};
	rt.insert_node(id, 234);
	rt.print();

  std::cout<<fs::file_size("hello.txt")<<std::endl;

  storage::store_file("hello.txt");



    


  //std::cout<<rt<<std::endl;
  
	kademlia::message msg{kademlia::messageType::PING};

	if (msg.type() == kademlia::messageType::PING) {
		std::cout << "\n\nPingPing\n\n";
	}

	int a = 8;
	float d = 9.687;
	char b = 'b';
	std::string e = "baka";

	std::cout << a << "\n" << d << "\n" << b << "\n" << e <<"\n\n";

	msg << a << d << b << e;
	
	a = 10;
	d = 1.34;
	b = 'a';
	e = "stupid";
	std::cout << a << "\n" << d << "\n" << b << "\n" << e <<"\n\n";

	msg >> e >> b >> d >> a;

	std::cout << a << "\n" << d << "\n" << b << "\n" << e << "\n\n";

	*/

	std::pair<std::string, uint16_t> endpoint;
	endpoint.first = "127.0.0.1";

	if (argc == 1) {
		endpoint.second = 1208;
		PORT = 2103;
	}

	else{
		endpoint.second = 2103;
		PORT = 1208;
		
	}

	network::client client{PORT};

	std::thread receive_thread([&] {client.receive();});
	
	kademlia::message msg{kademlia::messageType::PING};

	int a = 8;
	float d = 9.687;
	char b = 'b';
	std::string e = "baka";

	msg << a << d << b << e;

	if(argc == 1){
		std::cout << "come on";
		client.send(endpoint, msg);
	}
	receive_thread.join();

	/*
	kademlia::message msg{kademlia::messageType::PING};
	int a = 8;
	float d = 9.687;
	char b = 'b';
	std::string e = "baka";

	msg << a << d << b << e;
	std::string data;

	{
		std::stringstream stream_data;
		boost::archive::binary_oarchive oa(stream_data);
		oa << msg;
		data = stream_data.str();
	}

	kademlia::message recov_msg;

	{
		std::stringstream stream_data;
		for (int i = 0; i < data.size(); i++) {
			stream_data << data[i];
		}
		boost::archive::binary_iarchive ia(stream_data);
		ia >> recov_msg;
	}

	if (recov_msg.type() == kademlia::messageType::PING) {
		std::cout << "\nIt is ping";
	}

	int f; float g; char h; std::string i;

	recov_msg >> i >> h >> g >> f;

	std::cout << "\n" << f << "\n" << g << "\n" << h << "\n" << i << "\n\n";
	*/

	return 0;
}
