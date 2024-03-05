
//header file to handle all the network operations (receive, send)
#ifndef NETWORKING_H
#define NETWORKING_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include <map>
#include <utility>

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <boost/bind/bind.hpp>

#include <kademlia/message.h>
#include <routing_table.h>
#include <types.h>

using namespace boost;

//local ip of a device always constant only for testing purpose in practical should hold own ip and port it is binding
const std::string IPADDRESS = "127.0.0.1";


namespace kademlia{
	namespace network{
		class client{
			public:
				client(const uint16_t port, std::string self_id);
				~client();
				void receive();
				void send(const std::pair<std::string, uint16_t> endpoint, kademlia::message& msg);

				void send_ping_request(endpoint_type endpoint);
				void send_store_request(endpoint_type endpoint, kademlia::ID hash, std::string content);
				void send_find_value_request(endpoint_type endpoint, kademlia::ID piece_id);
				void send_find_node_request(endpoint_type endpoint, kademlia::ID node_id);
			private:
				void handle_receive(const system::error_code& error, size_t bytes_tranferred);
				void wait();
				void store(std::pair<std::string,std::string> key_value);
				void find_value(std::pair<std::string,uint16_t> endpoint, std::string node_id);
				void ping(std::pair<std::string,uint16_t> endpoint);
				void find_node(std::pair<std::string,uint16_t> endpoint, std::string node_id);

				void handle_ping_request(const endpoint_type endpoint, kademlia::message msg);
				void handle_find_node_request(const endpoint_type endpoint, kademlia::message msg);
				void handle_find_value_request(const endpoint_type endpoint, kademlia::message msg);
				void handle_store_request(const endpoint_type endpoint, kademlia::message msg);

				void send_find_id_request(endpoint_type endpoint, kademlia::ID node_id);
			//	void event_loop();
			private:
				uint16_t PORT;
				kademlia::ID self_id;
				asio::io_context io_context;
				asio::ip::udp::socket socket{io_context};
				std::array<char, 100*1024> recv_buffer;
				asio::ip::udp::endpoint remote_endpoint;
				std::map<kademlia::ID,kademlia::message, kademlia::ID_comparer> responses;
				kademlia::routing_table routing_table;
		};
	}
}

#endif

