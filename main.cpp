#include<iostream>
#include<random>
#include<functional>
#include<bitset>
#include<string>
#include"./include/routing_table.h"
#include"./include/id.h"

int main(){
  //std::cout<<generate_id()<<std::endl;

  RoutingTable rt{"101100"};


  rt.print();
  ID id{"101101"};
  rt.insert_node(id, 234);
  rt.print();

  //std::cout<<rt<<std::endl;

  return 0;
}
