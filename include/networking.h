
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

const std::string IPADDRESS = "127.0.0.1";
extern uint16_t PORT;

enum class input_command_type{
  HELP,
  STORE,
  RETRIVE,
  LS,
  FILE_STATUS
};

namespace network{
	class client{
		public:
			client();
			void send(const std::pair<std::string, uint16_t> endpoint,const kademlia::message& msg);
		private:
			void handle_receive(const system::error_code& error, size_t bytes_tranferred);
			void wait();
      void event_loop();
		private:
			asio::io_context io_context;
			asio::ip::udp::socket socket{io_context};
			std::vector<char> recv_buffer;
			asio::ip::udp::endpoint remote_endpoint;
	};
}

#endif
