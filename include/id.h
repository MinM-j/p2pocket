#ifndef ID_H
#define ID_H

#include <config.h>

#include <functional>


namespace kademlia{

using ID = std::bitset<NO_OF_BIT>;

ID create_id(std::string str);

struct  ID_comparer{
bool operator()( const ID& id1,  const ID& id2)const;
};

//using id_comparer_type= std::function<bool(const ID& , const ID&)>;
//extern id_comparer_type ID_comparer;

ID generate_id();
std::string generate_random_bit(int no_of_bit);
}//namespace kademlia

#endif //ID_H
