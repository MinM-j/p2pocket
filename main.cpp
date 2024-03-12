#include<iostream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>

#include<debug.h>
#include<stdlib.h>
using namespace std;


#include<crypto.h>

std::string get_dir_name(fs::path path){
  std::cout<<"path: "<<path<<std::endl<<std::endl;

  if(path.filename().empty()){
    return path.parent_path().filename();
    std::cout <<path.parent_path().filename()<<std::endl;
  }
  std::cout<<path.filename()<<std::endl;
  return path.filename();

}
int main(int argc, char* argv[]){
  //timer_debug();
  //routing_table_debug();
  //std::string src{"hello world"};
  //client_debug();
  //event_loop();

  init_node(argc, argv);

  /*
  fs::path path1{"hello/fromt/the/other/world"};
  fs::path path2{"./the/other/world/"};
  fs::path path3{"./../include"};
  fs::path path4{"./../../the/other/world/"};

  auto curr=fs::current_path();
  std::cout<< curr/path3<<std::endl;

  std::cout<<"curr "<<fs::current_path()<<std::endl;
  std::cout<<std::endl;
  std::cout<<std::endl;
  std::cout<<"ff"<<std::endl;


  path1 = fs::absolute(path1);
  path2 = fs::absolute(path2);
  path3 = fs::absolute(path3);
  path4 = fs::absolute(path4);

  std::cout<<std::endl;
  std::cout<<std::endl;
  get_dir_name(path1);
  get_dir_name(path2);
  get_dir_name(path3);
  get_dir_name(path4);

  */

  return 0;
}


