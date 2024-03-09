#include<iostream>
#include<string>
#include<bitset>
#include<random>
#include<list>
#include<array>
#include<filesystem>
#include <algorithm>
#include<id.h>

#include<config.h>
#include<types.h>

#include<routing_table.h>

namespace fs = std::filesystem;
//using namespace kademlia::routing_table;

std::ostream& operator<<(std::ostream& out , const kademlia::routing_table::k_bucket& table){
  //for(const auto& bucket: table.m_k_buckets){
  out<<"\n["; 
  for(const auto& node: table){
    out<<node.first<<":"<<node.second.second<<", ";
  }
  out<<"]"<<std::endl;
  return out;
}

namespace kademlia{
routing_table::routing_table(std::string id)
: m_self_id{kademlia::create_id(id)}
{
  m_bucket_size= CURRENT_BUCKET_SIZE;
}

routing_table::routing_table()=default;

std::ostream& operator<<(std::ostream& out , const routing_table& table){
  //out<< "self: "<< table.m_self_id.to_string()<<std::endl;
  //for(const auto& bucket: table.m_k_buckets){
  out<<std::endl;
  for(int i = 0; i< table.m_k_buckets.size(); i++){
    if(table.m_k_buckets[i].empty())
      continue;
    out<<i<<": ["; 
    for(const auto& node: table.m_k_buckets[i]){
      out<<node.first<<":"<<node.second.second<<", ";
    }
    out<<"]"<<std::endl;
  }
  out<<std::endl;
  return out;
}

routing_table::k_bucket  routing_table::find_node(kademlia::ID node_id){
  std::cout<<"FIND NODE"<<std::endl;
  //return list of nodes closes to given node if not found
  //else return list of closes nodes
  //TOOD: for find_node_request call handle_communication after this function call
  ////this is necessary for bootstrapping

  auto k_bucket_index = find_k_bucket_index(node_id);
  auto& bucket = m_k_buckets[k_bucket_index];
  //std::cout<<"k_bucket_index: "<<k_bucket_index<<std::endl;
  //std::cout<<"node_id: "<<node_id<<std::endl;

  auto it = find_node_in_bucket(bucket, node_id) ;
  //std::cout<<"FIND NODE"<<std::endl;


  bool is_node_found = !(it == bucket.end());
  if(is_node_found){
    std::cout<<"node found: "<<node_id<<std::endl;
    k_bucket kb;
    kb.push_back(*it);
    return kb;
  }
  if(bucket.size()>1){
    std::cout<<"bucket size greater than 2\n"<<node_id<<std::endl;
    return bucket;
  }

  std::vector<value_type> all_nodes;

  std::cout<<"total_nodes find_node: "<<all_nodes.size()<<std::endl;
  for(auto k_bucket: m_k_buckets){
    for (auto node: k_bucket){
        all_nodes.push_back(node);
    }
  }


  std::sort(all_nodes.begin(), all_nodes.end(),
            [node_id](const auto& lhs, const auto& rhs){
            //comparer returns true if first argument is less than second
            //return kademlia::ID_comparer((node_id ^ lhs.first) , (node_id ^ rhs.first));
            return (node_id ^ lhs.first).to_ulong()<  (node_id ^ rhs.first).to_ulong();
            });

  k_bucket kb;
  auto no_of_nodes=std::min(5,static_cast<int>(all_nodes.size()));
  kb.insert(kb.begin(),all_nodes.begin(),std::next(all_nodes.begin(),no_of_nodes));
  return kb;
}

void routing_table::insert_node( kademlia::ID node_id, node_type node ){

  auto k_bucket_index = find_k_bucket_index(node_id);
  //std::cout<<"k_bucket_index: "<<k_bucket_index<<std::endl;
  auto& bucket = m_k_buckets[k_bucket_index];

  //std::cout<<k_bucket_index<<std::endl;

  if(bucket.size() >= CURRENT_BUCKET_SIZE){
    // TODO: ping least recently seen nodes in the k_bucket and do accordingly
    // instead put this node into replacement cache, MORE: sec 4.1
    std::cout<<"hello"<<std::endl;
  }else{
    auto it = find_node_in_bucket(bucket, node_id) ;
    bool is_new_node = it == bucket.end();
    //std::cout<< "is_new_node: "<<std::boolalpha<<is_new_node<<std::endl;

    if(is_new_node){
      bucket.push_back(std::make_pair(node_id, node));
      std::cout<<"inserted node with id: "<<node_id<<std::endl;
    }
  }

  std::cout<<*this;
}

void routing_table::remove_node(kademlia::ID node_id){
  auto k_bucket_index = find_k_bucket_index(node_id);
  auto& bucket = m_k_buckets[k_bucket_index];

  auto it = find_node_in_bucket(bucket, node_id); 
  if(it == bucket.end()){
    // node not in k_bucket
  }
  else{
    bucket.erase(it);
  }
}

void routing_table::handle_communication( kademlia::ID node_id, node_type node ){
  //ig
  this->insert_node(node_id,node);
}

routing_table::k_bucket::const_iterator  routing_table::find_node_in_bucket(const routing_table::k_bucket& bucket, const kademlia::ID& node_id){
  return std::find_if(bucket.cbegin(), bucket.cend(),
                      [&node_id](const value_type curr_node){
                      auto curr_node_id = curr_node.first;
                      return node_id == curr_node_id;
                      });
}

bool routing_table::is_node_in_bucket(const routing_table::k_bucket& bucket, const kademlia::ID& node_id){
  auto it = find_node_in_bucket(bucket, node_id) ;
  bool is_node_found = !(it == bucket.end());
  return is_node_found;

}
std::size_t routing_table::find_k_bucket_index(const kademlia::ID& peer_id){
  std::size_t i = NO_OF_BIT;

  // find the first non-matching bit
  //std::cout<<m_self_id<<std::endl;
  //std::cout<<peer_id<<std::endl;
  while((i--)>0 &&  m_self_id[i]== peer_id[i]){
    // the `i--` is necessary during reverse iteration using `std::size_t` counter( i in this case).
    // https://stackoverflow.com/a/276056
  }
  return NO_OF_BIT - 1 - i;
}

}//namespace kademlia
