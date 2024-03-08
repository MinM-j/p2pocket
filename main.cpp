#include<debug.h>
#include<iostream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>

using namespace std;

int mmain()
{
  string s = "uFFFFFFFF";
  std::cout<<s<<std::endl;
  stringstream ss;
  ss << hex << s;
  unsigned n;
  ss >> n;
  bitset<32> b(n);
  // outputs "00000000000000000000000000001010"
  cout << b.to_string() << endl;
}

int main(int argc, char* argv[]){
  //timer_debug();

  //routing_table_debug();


  //client_debug();
  event_loop();
  return 0;
}
