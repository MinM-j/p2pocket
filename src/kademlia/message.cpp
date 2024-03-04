#include <kademlia/message.h>
#include <networking.h>

kademlia::message::message(const messageType msg_type, ID id){
	header.self_id = id;
	header.msg_type = msg_type;
}

kademlia::messageType kademlia::message::type() const{
	return this->header.msg_type;
}

size_t kademlia::message::size() const{
///* TODO:  <04-03-24, yourname> */
	return sizeof(messageType) + body.size();
}
