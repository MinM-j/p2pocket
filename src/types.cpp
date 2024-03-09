#include <types.h>

std::ostream& operator<<(std::ostream& out,const kademlia::endpoint_type& ep){
  out<< ep.first<<":"<<ep.second<<std::endl;
  return out;
}



//std::ostream& operator<<(std::ostream& out , const kademlia::k_bucket& table){
//  //for(const auto& bucket: table.m_k_buckets){
//  out<<"\n["; 
//  for(const auto& node: table){
//    out<<node.first<<":"<<node.second.second<<", ";
//  }
//  out<<"]"<<std::endl;
//  return out;
//}
