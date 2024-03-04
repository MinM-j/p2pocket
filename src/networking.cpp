#include <networking.h>
#include<filesystem>
#include<stdexcept>
#include<storage.h>

/*
//make this dir in first run
constexpr fs::path data_dir{"p2pocket/data/"};
constexprt std::size_t MAX_ALLOCATED_STORAGE = 100*1024*1024; //100 mb for now (temp)
*/

//namespace fs = std::filesystem;

//this should start the io_context and listen for every incoming messages
kademlia::network::client::client(const uint16_t port, std::string id) : PORT{port}, self_id{id}, routing_table{"sth"}{
	socket.open(asio::ip::udp::v4());
	socket.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(IPADDRESS), PORT));
}

void kademlia::network::client::receive(){
	wait();
	io_context.run();
}

//basically a function for asynchronous receive operation
void kademlia::network::client::wait(){
	socket.async_receive_from(asio::buffer(recv_buffer),
			remote_endpoint,
			boost::bind(&client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

//function to handle the receive where the message will be parsed and required action will be taken
void kademlia::network::client::handle_receive(const system::error_code& error, size_t bytes_tranferred){
	if (error) {
		std::cout << "Receive failed : " << error.message() << std::endl;
		return;
	}

	std::cout << "\n\nData received from(" << remote_endpoint.address() << ":" << remote_endpoint.port() << ") byte received: " << bytes_tranferred <<"\n\n";

	std::string data(recv_buffer.begin(), recv_buffer.begin()+bytes_tranferred);
	std::cout << data << std::endl;
	std::cout << "\n\n Bytes receive : " << bytes_tranferred << "\n\n";

	std::stringstream stream_data;
	stream_data << data;
	
	kademlia::message msg;

	{
		boost::archive::binary_iarchive ia(stream_data);
		ia >> msg;
	}

	endpoint_type endpoint(remote_endpoint.address().to_string(),remote_endpoint.port());

	switch(msg.type()){
		case kademlia::messageType::PING:
			handle_ping_request(endpoint, msg);
			break;
		case kademlia::messageType::STORE:
			handle_store_request(endpoint, msg);
			break;
		case kademlia::messageType::FIND_NODE:
			handle_find_node_request(endpoint, msg);
			break;
		case kademlia::messageType::FIND_VALUE:
			handle_find_value_request(endpoint, msg);
			break;

		case kademlia::messageType::PING_RESPONSE:
		case kademlia::messageType::STORE_RESPONSE:
		case kademlia::messageType::FIND_NODE_RESPONSE:
		case kademlia::messageType::FIND_VALUE_RESPONSE:
			responses.insert({msg.header.self_id, msg});
			break;

	}
	wait();
}

void kademlia::network::client::send(const std::pair<std::string, uint16_t> endpoint,const kademlia::message& msg){
	asio::ip::udp::endpoint receiver_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(endpoint.first), endpoint.second);
	boost::system::error_code send_error;
	std::string data;
	{
		std::stringstream stream_data;
		boost::archive::binary_oarchive oa(stream_data);
		oa << msg;
		data = stream_data.str();
	}
	auto sent = socket.send_to(asio::buffer(data), receiver_endpoint, 0, send_error);
	std::cout << sent << " bytes sent to(" << receiver_endpoint.address() << ":" << receiver_endpoint.port() << ")\n\n";

}

kademlia::network::client::~client(){
	socket.close();
}

void kademlia::network::client::send_ping_request(endpoint_type endpoint){
	message request{messageType::PING, self_id};
	send(endpoint, request);
}

void kademlia::network::client::send_store_request(endpoint_type endpoint, kademlia::ID piece_hash, std::string content){
	message request{messageType::STORE, self_id};
	//TODO SERIALIZE: pair
	request<<content<<piece_hash;
	send(endpoint,request);
}

void kademlia::network::client::send_find_id_request(endpoint_type endpoint, kademlia::ID node_id){
	message request{messageType::FIND_NODE, self_id};
	request<<node_id;

	send(endpoint, request);
}

void kademlia::network::client::send_find_value_request(endpoint_type endpoint, kademlia::ID piece_id){
	send_find_node_request(endpoint, piece_id);
}

void kademlia::network::client::send_find_node_request(endpoint_type endpoint, kademlia::ID node_id){
	send_find_node_request(endpoint, node_id);
}

void kademlia::network::client::handle_ping_request(const endpoint_type endpoint,const kademlia::message msg){

	routing_table.handle_communication(msg.header.self_id);

	kademlia::message response{kademlia::messageType::PING_RESPONSE,self_id};
	send(endpoint,response);
}
//TODO: handle storage query

void kademlia::network::client::handle_store_request(const endpoint_type endpoint,const kademlia::message msg){
	kademlia::message response{kademlia::messageType::STORE_RESPONSE,self_id};
	routing_table.handle_communication(msg.header.self_id);

	try{
		kademlia::storage::store_piece(msg);
		response<<"success";
	}catch(...){
		response<<"error";
	}
	send(endpoint,response);

}

void kademlia::network::client::handle_find_node_request(const endpoint_type endpoint,const kademlia::message msg){
	ID node_to_find;
	msg >> node_to_find;

	routing_table.handle_communication(msg.header.self_id);
	const kademlia::routing_table::k_bucket  nodes = routing_table.find_node(node_to_find);

	kademlia::message response{kademlia::messageType::FIND_NODE_RESPONSE,self_id};
	response<<nodes;
	send(endpoint, response);
}


void kademlia::network::client::handle_find_value_request(const endpoint_type endpoint,const kademlia::message msg){
	ID piece_id;
	msg>>piece_id;

	routing_table.handle_communication(msg.header.self_id);
	std::string content =kademlia::storage::find_file_piece(piece_id);

	kademlia::message response{kademlia::messageType::FIND_VALUE_RESPONSE,self_id};

	if(!content.empty()){
		response << content;
	}
	send(endpoint, response);
}
