#include<iostream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>

#include<debug.h>
using namespace std;



int main(int argc, char* argv[]){
  //timer_debug();

  //routing_table_debug();

  //init_node(argc, argv);

  //client_debug();
  //event_loop();

auto byte= kademlia::generate_random_bit(128);

  std::cout<<byte<<std::endl;
  return 0;
}
