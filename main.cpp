#include<iostream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>

#include<debug.h>
using namespace std;


#include<crypto.h>

int main(int argc, char* argv[]){
  //timer_debug();

  //routing_table_debug();

  //init_node(argc, argv);

  std::string src{"hello world"};

  crypto_init(kademlia::project_path / "test_encryption");
  auto cipher= encrypt(src);
  auto decipher = decrypt(cipher);
  std::cout<<cipher<<std::endl
    << decipher<<std::endl;
  //client_debug();
  //event_loop();
  return 0;
}


