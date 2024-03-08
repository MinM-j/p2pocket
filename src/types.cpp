#include <types.h>

namespace kademlia{
} //namespace kademlia


std::ostream& operator<<(std::ostream& out,const kademlia::endpoint_type& ep){
  out<< ep.first<<":"<<ep.second<<std::endl;
  return out;
}

