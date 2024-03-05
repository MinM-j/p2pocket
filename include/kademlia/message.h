#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include<sstream>
#include <vector>
#include <cstring>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/bitset.hpp>


#include <config.h>
#include <id.h>
#include <routing_table.h>

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
public:
  messageHeader header{};
  std::vector<uint8_t> body;

public:
  message() = default;

  //constructor where the type of message is defined
  message(const messageType msg_type, ID self_id);

  //returns the type of message
  messageType type() const;

  //returns the size of message 
  size_t size() const;

public:
  class k_bucket_serialization {
  private:
    using k_bucket=kademlia::routing_table::k_bucket;
    std::string serialized;
  public:
    k_bucket s;

    k_bucket_serialization()=default;
    k_bucket_serialization(const k_bucket& data): s{data}{}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
      ar & s;
    }

    std::string serialize(){
      std::stringstream stream_data;
      boost::archive::binary_oarchive oa(stream_data);
      oa << *this;
      return stream_data.str();
    }

  };

  //to create the body of message of different data types
  //k__bucket
  //ID
  //string const char *

  friend message& operator << (message& msg , kademlia::routing_table::k_bucket data){
    k_bucket_serialization serialized_data{data};
    std::string serialized_str = serialized_data.serialize();
    msg<<serialized_str;
    return msg;
  }

  friend message& operator << (message& msg,ID id){
    msg<<id.to_string();
    return msg;
  }

  friend message& operator << (message& msg,std::string data ){
    size_t i = msg.body.size();
    size_t data_size = data.size();
    msg.body.resize(i + data_size);
    memcpy(msg.body.data()+i, data.data(), data_size);
    msg.header.size = msg.size();
    return msg;
  }


  //==============================

  friend message& operator >> (message& msg , kademlia::routing_table::k_bucket& data){
    k_bucket_serialization bucket_serializer{};
    std::stringstream stream_data;

    std::string raw_data{msg.body.begin(), msg.body.end()};

    stream_data << raw_data;
    boost::archive::binary_iarchive ia(stream_data);
    ia >> bucket_serializer;

    data= bucket_serializer.s;

    return msg;
  }

  friend message& operator >> (message& msg,ID& id){
    size_t i= msg.body.size()-NO_OF_BIT;
    std::string id_str{msg.body.begin() + i, msg.body.end()};

    msg.body.resize(i);

    id = kademlia::create_id(id_str);

    return  msg;
  }

  friend message& operator >> (message& msg,std::string& data ){
    data.assign(msg.body.begin(),msg.body.end());
    return msg;
  }


  friend class boost::serialization::access;
  template<typename Archive>
  void serialize(Archive& ar, const unsigned int version){
    ar & header;
    ar & body;
  }

};
}

#endif
