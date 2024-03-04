
//header file to handle all the network operations (receive, send)
#ifndef NETWORKING_H
#define NETWORKING_H

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <boost/bind/bind.hpp>
#include <kademlia/message.h>
#include <iostream>
#include <stdint.h>
#include <vector>

using namespace boost;

//local ip of a device always constant only for testing purpose in practical should hold own ip and port it is binding
const std::string IPADDRESS = "127.0.0.1";

enum class input_command_type{
  HELP,
  STORE,
  RETRIVE,
  LS,
  FILE_STATUS
};

namespace kademlia{
	namespace network{
		class client{
			public:
				client(const uint16_t port);
				~client();
				void receive();
				void send(const std::pair<std::string, uint16_t> endpoint,const kademlia::message& msg);

			private:
				void handle_receive(const system::error_code& error, size_t bytes_tranferred);
				void wait();
				void store(std::pair<std::string,std::string> key_value);
				void find_value(std::pair<std::string,uint16_t> endpoint, std::string node_id);
				void ping(std::pair<std::string,uint16_t> endpoint);
				void find_node(std::pair<std::string,uint16_t> endpoint, std::string node_id);
			//	void event_loop();
			private:
				uint16_t PORT;
				asio::io_context io_context;
				asio::ip::udp::socket socket{io_context};
				std::array<char, 100*1024> recv_buffer;
				asio::ip::udp::endpoint remote_endpoint;
		};
	}
}

#endif
