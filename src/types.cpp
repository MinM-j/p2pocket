#include <types.h>
#include<sstream>

std::ostream& operator<<(std::ostream& out,const kademlia::endpoint_type& ep){
  out<< ep.first<<":"<<ep.second;
  return out;
}

std::istream& operator>>(std::istream& in, kademlia::endpoint_type& ep){
  //std::cout<<"overload:"<<in.str()<<std::endl;
  std::string temp_ep;
  in>>temp_ep;
  std::stringstream sstream;
  sstream<<temp_ep;

  std::getline(sstream, ep.first, ':');
  sstream>>ep.second;

  return in;
}

/*
std::stringstream& operator<<(std::sstream& in, kademlia::endpoint_type& ep){
  std::getline(in, ep.first, ':');
  in>>ep.second;

  std::cout<<ep<<std::endl;
  return in;
}


*/

//std::ostream& operator<<(std::ostream& out , const kademlia::k_bucket& table){
//  //for(const auto& bucket: table.m_k_buckets){
//  out<<"\n["; 
//  for(const auto& node: table){
//    out<<node.first<<":"<<node.second.second<<", ";
//  }
//  out<<"]"<<std::endl;
//  return out;
//}
