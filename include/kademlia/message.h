#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <vector>
#include <cstring>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace kademlia{
	//message type necessary for header
	enum class messageType : uint32_t{
		PING,
		STORE,
		FIND_NODE,
		FIND_VALUE,
	};

	//message header that holds the message type and total bytes of message
	struct messageHeader{
		messageType msg_type;
		uint32_t size = 0;

		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version){
			ar & msg_type;
			ar & size;
		}
	};

	//message contains header and body (body is of type uint8_t to make in handle in a byte)
	struct message{
		messageHeader header{};
		std::vector<uint8_t> body;

		message() = default;

		//constructor where the type of message is defined
		message(const messageType msg_type);

		//returns the type of message
		messageType type() const;

		//returns the size of message 
		size_t size() const;

		//to create the body of message of different data types
		template<typename dataType>
		friend message& operator << (message& msg,const dataType& data){
			size_t i = msg.body.size();
			msg.body.resize(i + sizeof(dataType));
			memcpy(msg.body.data()+i, &data, sizeof(dataType));
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

	//Namespace to hold all the functions that messages
	namespace message_protocol{
		void ping(std::pair<std::string,uint16_t> endpoint);
		void store(std::pair<std::string,std::string> key_value);
		void find_node(std::pair<std::string,uint16_t> endpoint, std::string node_id);
		void find_value(std::pair<std::string,uint16_t> endpoint, std::string node_id);
	}

}

#endif
