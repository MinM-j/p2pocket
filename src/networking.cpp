
#include <networking.h>

network::client::client(){
	recv_buffer.reserve(64 * 1024);
	socket.open(asio::ip::udp::v4());
	socket.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(IPADDRESS), PORT));
	wait();
	io_context.run();
}

void network::client::wait(){
	socket.async_receive_from(asio::buffer(recv_buffer),
			remote_endpoint,
			boost::bind(&client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void network::client::handle_receive(const system::error_code& error, size_t bytes_tranferred){
	if (error) {
		std::cout << "Receive failed : " << error.message() << std::endl;
		return;
	}
	wait();
}

void network::client::send(const std::pair<std::string, uint16_t> endpoint,const kademlia::message& msg){

}
