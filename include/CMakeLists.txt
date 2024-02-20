set(Path ${CMAKE_CURRENT_SOURCE_DIR})
set(Hello ${Path}/hello.h)
set(Networking ${Path}/networking.h)

add_subdirectory(${Path}/kademlia)

set(Headers ${Hello} ${Kademlia} ${Networking} PARENT_SCOPE)
