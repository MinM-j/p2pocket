#ifndef ID_H
#define ID_H

#include"config.h"

//class ID{
//  private:
//  using id_type = std::bitset<NO_OF_BIT>;
//  const id_type m_id;
//
//  public:
//  ID(std::string id): m_id(id){}
//  //ID():m_id(){}
//  bool operator[](size_t i) const{
//    return m_id[i];
//  }
//
//  //  string || string_view ??
//  std::string to_string()const{
//    return  m_id.to_string();
//  }
//
//};

using ID = std::bitset<NO_OF_BIT>;
using id_type = ID;

namespace temp_id_namespace{
   ID generate_id(){
    std::random_device random_device;
    std::mt19937 generator;
    generator.seed(random_device());
    constexpr int generator_bit= 32; //size of std::mt19937()??

    id_type new_id;
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
    return ID{new_id.to_string()};

  }
}//namespace ID

#endif //HELLO_H
