set(Path ${CMAKE_CURRENT_SOURCE_DIR})
set(Networking 
${Path}/networking.cpp 
${Path}/interface.cpp 
${Path}/id.cpp 
${Path}/storage.cpp 
${Path}/routing_table.cpp 
${Path}/timer.cpp 
${Path}/types.cpp 
)

set(Crypto
	${Path}/crypto.cpp
)

add_subdirectory(${Path}/kademlia)

set(Sources ${Kademlia} ${Networking} ${Crypto} PARENT_SCOPE)

