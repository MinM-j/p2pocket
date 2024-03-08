set(Path ${CMAKE_CURRENT_SOURCE_DIR})
set(Networking 
${Path}/networking.h
${Path}/id.h
${Path}/interface.h
${Path}/routing_table.h
${Path}/sha1.h
${Path}/storage.h
${Path}/types.h
${Path}/timer.h
)

add_subdirectory(${Path}/kademlia)

set(Headers ${Kademlia} ${Networking} PARENT_SCOPE)
