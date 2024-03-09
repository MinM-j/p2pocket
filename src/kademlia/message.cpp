#include <kademlia/message.h>
#include <networking.h>

namespace kademlia{
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

std::ostream& operator<<(std::ostream& out, messageType type){
  switch(type){
    case messageType::PING:
      out<<"PING";
      break;
    case messageType::STORE:
      out<<"STORE";
      break;
    case messageType::FIND_NODE:
      out<<"FIND_NODE";
      break;
    case messageType::FIND_VALUE:
      out<<"FIND_VALUE";
      break;
    case messageType::PING_RESPONSE:
      out<<"PING_RESPONSE";
      break;
    case messageType::STORE_RESPONSE:
      out<<"STORE_RESPONSE";
      break;
    case messageType::FIND_NODE_RESPONSE:
      out<<"FIND_NODE_RESPONSE";
      break;
    case messageType::FIND_VALUE_RESPONSE:
      out<<"FIND_VALUE_RESPONSE";
      break;
  }
  return out;
}

}//namespace kademlia
