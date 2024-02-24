#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include<iostream>
#include<string>
#include<bitset>
#include<random>
#include<list>
#include<array>
#include<filesystem>

#include"./id.h"
#include"./config.h"

/*
 * note: In the ID | std::bitset<N>, while accessing individual bits, 
 * the indexing is done from LSB to MSB, i.e. index 0 denotes lsb.
 * Similary, in `k_buckets_` of RoutingTable, lower index represents 
 * less common prefixes.
 * For instance: For id: "101010" (owner) and "100010" have 2 common 
 * prefixes. Therefore in the routing table of "101010", "100010" will
 * reside in the index 2 k_bucket.
 * Each K-bucket is a `std::list` and the head of the list contains most
 * recently seen node and the tail of the list contains least recently seen node.
 */ 

namespace fs = std::filesystem;

class RoutingTable {
  private: 
    using NodeType = int; //temp
    using value_type=std::pair<ID, NodeType>;
    using k_bucket = std::list<value_type>;
    using k_buckets = std::array< k_bucket,NO_OF_BIT>;
    //static constexpr fs::path table_path{"./data/routing_table"}; //extension??

  private:
    const ID m_self_id;
    //This will be set from config file
    std::size_t m_bucket_size;
    k_buckets m_k_buckets;
  public: 
    RoutingTable(std::string id)
    : m_self_id{id}
    {
    m_bucket_size= CURRENT_BUCKET_SIZE;
    }

    void print(){
      for(int i = 0; i<m_k_buckets.size(); i++){
        std::cout<<i<<" -> "<<m_k_buckets[i].size()<<std::endl;
      }
      std::cout<<"================"<<std::endl;
    }
    friend std::ostream& operator<<(std::ostream& out , const RoutingTable& table){
      out<< "self: "<< table.m_self_id.to_string()<<std::endl;
      //for(const auto& bucket: table.m_k_buckets){
      for(int i = 0; i< table.m_k_buckets.size(); i++){
        out<<i<<": ["; 
        for(const auto& node: table.m_k_buckets[i]){
          out<<node.first<<", ";
        }
        out<<"]"<<std::endl;
      }
      return out;
    }


    void insert_node( ID node_id, NodeType node ){
      std::cout<<"inserting  node with id: "<<node_id<<std::endl;

      auto k_bucket_index = find_k_bucket_index(node_id);
      auto& bucket = m_k_buckets[k_bucket_index];

      std::cout<<k_bucket_index<<std::endl;

      std::cout<<bucket.size()<<std::endl;
      if(bucket.size() >= CURRENT_BUCKET_SIZE){
        // TODO: ping least recently seen nodes in the k_bucket and do accordingly
        // instead put this node into replacement cache, MORE: sec 4.1
        std::cout<<"hello"<<std::endl;
      }else{

         auto it = find_node_in_bucket(bucket, node_id) ;
         bool is_new_node = it == bucket.end();
         std::cout<<is_new_node<<" :is_new_node"<<std::endl;

        if(is_new_node){
          bucket.push_back(std::make_pair(node_id, node));
          std::cout<<"inserted node with id: "<<node_id<<std::endl;
        }
      }
    }

    void remove_node(ID node_id){
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

  private: 
    k_bucket::const_iterator  find_node_in_bucket(const k_bucket& bucket, const ID& node_id){
      return std::find_if(bucket.cbegin(), bucket.cend(),
                        [&node_id](const value_type curr_node){
                        auto curr_node_id = curr_node.first;
                        return node_id == curr_node_id;
                        });
    }

    std::size_t find_k_bucket_index(const ID& peer_id){
      std::size_t i = NO_OF_BIT;

      // find the first non-matching bit
      while((i--)>0 &&  m_self_id[i]== peer_id[i]){
        // the `i--` is necessary during reverse iteration using `std::size_t` counter( i in this case).
        // https://stackoverflow.com/a/276056
      }
      return NO_OF_BIT - 1 - i;
    }

};




#endif // ROUTING_TABLE_H

