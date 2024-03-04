
#include <kademlia/message.h>
#include <networking.h>

kademlia::message::message(const messageType msg_type){
	header.msg_type = msg_type;
}

kademlia::messageType kademlia::message::type() const{
	return this->header.msg_type;
}

size_t kademlia::message::size() const{
	return sizeof(messageType) + body.size();
}

void::kademlia::message_protocol::ping(std::pair<std::string,uint16_t> endpoint){
	message ping{messageType::PING};
	
}
