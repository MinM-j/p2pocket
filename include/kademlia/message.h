#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <vector>

namespace kademlia{
	enum class messageType : uint32_t{
		PING,
		STORE,
		FIND_NODE,
		FIND_VALUE,
	};

	struct messageHeader{
		messageType msg_type;
		uint32_t size = 0;
	};

	struct message{
		messageHeader header{};
		std::vector<uint8_t> body;
	};
}

#endif
