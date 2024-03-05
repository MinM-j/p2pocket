#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <type_traits>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/list.hpp>

#include <id.h>
#include<routing_table.h>



namespace kademlia{
//message type necessary for header
enum class messageType : uint32_t{
  PING,
  STORE,
  FIND_NODE,
  FIND_VALUE,
  PING_RESPONSE,
  STORE_RESPONSE,
  FIND_NODE_RESPONSE,
  FIND_VALUE_RESPONSE,
};

//message header that holds the message type and total bytes of message
struct messageHeader{
  messageType msg_type;
  kademlia::ID self_id;
  uint32_t size = 0;

  friend class boost::serialization::access;
  template<typename Archive>
  void serialize(Archive& ar, const unsigned int version){
    ar & msg_type;
    ar & self_id;
    ar & size;
  }
};

//message contains header and body (body is of type uint8_t to make in handle in a byte)
struct message{
  template<typename T>
  class toBeSerialized {
  private:
    T s;
    std::string serialized;
  public:
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
      ar & s;
    }

  };

  template<typename T>
  std::string serialize(const toBeSerialized<T>& data){
    std::stringstream stream_data;
    boost::archive::binary_oarchive oa(stream_data);
    oa << data;
    return stream_data.str();
  }

  messageHeader header{};
  std::vector<uint8_t> body;

  message() = default;

  //constructor where the type of message is defined
  message(const messageType msg_type, ID self_id);

  //returns the type of message
  messageType type() const;

  //returns the size of message 
  size_t size() const;

  //toBeSerialized<dataType> serialize_obj{data};
  //serialized_data = serialize(serialize_obj);

  //to create the body of message of different data types
  template<typename dataType>
  friend message& operator << (message& msg,const dataType& data){
    std::string serialized_data = 
      (std::is_same<dataType,ID>::value || std::is_same<dataType, routing_table::k_bucket>::value) 
      ? serialize(toBeSerialized<dataType>{data})
      : data;

    size_t i = msg.body.size();
    msg.body.resize(i + sizeof(dataType));
    //memcpy(msg.body.data()+i, &data, sizeof(dataType));
    std::memcpy(msg.body().data() + i, serialized_data.data(), serialized_data.size());
    msg.header.size = msg.size();
    return msg;
  }

  //to read from body
  template<typename dataType>
  friend message& operator >> (message& msg, dataType& data){
    size_t i = msg.body.size() - sizeof(dataType);
    memcpy(&data, msg.body.data()+i, sizeof(dataType));
    msg.body.resize(i);
    msg.header.size = msg.size();
    return msg;
  }

  friend class boost::serialization::access;
  template<typename Archive>
  void serialize(Archive& ar, const unsigned int version){
    ar & header;
    ar & body;
  }

};


#endif

