
#include <networking.h>

//this should start the io_context and listen for every incoming messages
network::client::client(const uint16_t port) : PORT{port}{
	recv_buffer.reserve(100 * 1024);
	socket.open(asio::ip::udp::v4());
	socket.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(IPADDRESS), PORT));
}

void network::client::receive(){
	wait();
	io_context.run();
}

//basically a function for asynchronous receive operation
void network::client::wait(){
	socket.async_receive_from(asio::buffer(recv_buffer),
			remote_endpoint,
			boost::bind(&client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

//function to handle the receive where the message will be parsed and required action will be taken
void network::client::handle_receive(const system::error_code& error, size_t bytes_tranferred){
	if (error) {
		std::cout << "Receive failed : " << error.message() << std::endl;
		return;
	}

	std::cout << "\n\nData received\n\n";

	std::cout << std::string(recv_buffer.begin(),recv_buffer.begin() + bytes_tranferred);
/*
	std::cout << "\n\n Bytes receive : " << bytes_tranferred << "\n\n";
	
	for (int i = 0; i < bytes_tranferred; i++)
		stream_data << recv_buffer[i];

	kademlia::message msg;

	{
		boost::archive::binary_iarchive ia(stream_data);
		ia >> msg;
	}

	if (msg.type() == kademlia::messageType::PING) {
		std::cout << "\nIt is ping";
	}

	int a; float d; char b; std::string e;

	msg >> e >> b >> d >> a;

	std::cout << a << "\n" << d << "\n" << b << "\n" << e << "\n\n";
	*/

	wait();
}

void network::client::send(const std::pair<std::string, uint16_t> endpoint,const kademlia::message& msg){
	asio::ip::udp::endpoint receiver_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(endpoint.first), endpoint.second);
	boost::system::error_code send_error;
	std::string data;
	{
		std::stringstream stream_data;
		boost::archive::binary_oarchive oa(stream_data);
		oa << msg;
		data = stream_data.str();
	}

	auto sent = socket.send_to(asio::buffer(data.data(), data.size()), receiver_endpoint, 0, send_error);
	std::cout << "\n\nData sent\n\nsize of data in bytes : " << data.size() << "\n\n";

//Todo:
	//data should be serialized before sending it can be achieved through boost::serialization as buffer only support POD


//	auto sent = socket.send_to(asio::buffer(,), receiver_endpoint, 0, send_error);
}
