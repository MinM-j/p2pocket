set(Path ${CMAKE_CURRENT_SOURCE_DIR})
set(Networking 
${Path}/networking.cpp 
${Path}/interface.cpp 
${Path}/id.cpp 
${Path}/storage.cpp 
)

add_subdirectory(${Path}/kademlia)

set(Sources ${Kademlia} ${Networking} PARENT_SCOPE)

