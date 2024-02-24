set(Path ${CMAKE_CURRENT_SOURCE_DIR})
set(Networking ${Path}/networking.h)

add_subdirectory(${Path}/kademlia)

set(Headers ${Kademlia} ${Networking} PARENT_SCOPE)
