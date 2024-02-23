#include<iostream>
#include<fstream>
#include<random>
#include<functional>
#include<bitset>
#include<string>
#include"./include/routing_table.h"
#include"./include/id.h"
#include"./include/storage/sha1.h"
#include"./include/storage/storage.h"

int main(){
  //std::cout<<generate_id()<<std::endl;


  std::cout<<fs::file_size("hello.txt")<<std::endl;

  storage::store_file("hello.txt");



    


  //std::cout<<rt<<std::endl;

  return 0;
}
