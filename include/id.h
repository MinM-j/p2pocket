#ifndef ID_H
#define ID_H

#include"config.h"



namespace kademlia{

using ID = std::bitset<NO_OF_BIT>;

ID create_id(std::string str);

struct  ID_comparer{
  bool operator()( const ID& id1,  const ID& id2)const {
     return id1.to_ulong() < id2.to_ulong();
  }
};

ID generate_id();

}//namespace ID

#endif //ID_H
