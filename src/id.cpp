#include <id.h>
#include <algorithm>


namespace  kademlia{
ID generate_id(){
  std::random_device random_device;
  std::mt19937 generator;
  generator.seed(random_device());
  constexpr int generator_bit= 32; //size of std::mt19937()??

  kademlia::ID new_id;
  for(int i = 0; i< NO_OF_BIT/generator_bit;i++){
    auto random= generator();
    new_id <<=generator_bit;
    new_id |= std::bitset<NO_OF_BIT>(random);
  }

  constexpr int remaining_bit_count = NO_OF_BIT % generator_bit;
  if(remaining_bit_count != 0){
    auto random = generator();
    random >>= (generator_bit - remaining_bit_count);
    new_id <<=remaining_bit_count;
    new_id |= std::bitset<NO_OF_BIT>(random);
  }
  return new_id;
}


ID create_id(std::string str){
  std::reverse(str.begin(), str.end());
  return ID{str};
}
}
