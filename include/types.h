#include<utility>
#include<iostream>
#include<cstdint>

#ifndef TYPES_H
#define TYPES_H
namespace kademlia{

using endpoint_type = std::pair<std::string,uint16_t>;


}// namespcae kademlia

std::ostream& operator<<(std::ostream& out,const kademlia::endpoint_type& ep);
std::istream& operator>>(std::istream& in, kademlia::endpoint_type& ep);
//std::stringstream& operator<<(std::sstream& in, kademlia::endpoint_type& ep);

#endif //TYPES_H

