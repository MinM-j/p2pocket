#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include<iostream>
#include <fstream>
#include<string>
#include<bitset>
#include<random>
#include<list>
#include<array>
#include<filesystem>
#include <algorithm>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/bitset.hpp>

#include<id.h>
#include<config.h>
#include<types.h>

/*
 * ID: and id `11010100` is stored as std::bitset.
 * The 0 index of id contains the MSB. and the `length-1` contains the LSB.
 */
/*
 * Similary, in `k_buckets_` of routing_table, lower index represents 
 * less common prefixes.
 * For instance: For id: "101010" (owner) and "100010" have 2 common 
 * prefixes. Therefore in the routing table of "101010", "100010" will
 * reside in the index 2 k_bucket.
 * Each K-bucket is a `std::list` and the head of the list contains most
 * recently seen node and the tail of the list contains least recently seen node.
 */ 

namespace fs = std::filesystem;

namespace kademlia{
class routing_table {
public:
  using node_type = endpoint_type; 
  using value_type=std::pair<kademlia::ID, node_type>;
  using k_bucket = std::list<value_type>;
  using k_buckets = std::array< k_bucket,NO_OF_BIT>;
  //static constexpr fs::path table_path{"./data/routing_table"}; //extension??

private:
  kademlia::ID m_self_id;
  //This will be set from config file
  std::size_t m_bucket_size;
  k_buckets m_k_buckets;
public: 
  routing_table();
  routing_table(std::string id);

  friend std::ostream& operator<<(std::ostream& out , const routing_table& table);

  k_bucket  find_node(kademlia::ID node_id);

  void insert_node( kademlia::ID node_id, node_type node );
  void remove_node(kademlia::ID node_id);

  void handle_communication( kademlia::ID node_id, node_type node );

  static k_bucket::const_iterator  find_node_in_bucket(const k_bucket& bucket, const kademlia::ID& node_id);
  static bool is_node_in_bucket(const k_bucket& bucket, const kademlia::ID& node_id);
private: 
  class k_buckets_serialization {
  private:
    using k_buckets=kademlia::routing_table::k_buckets;
    std::string serialized;
  public:
    k_buckets s;

    k_buckets_serialization()=default;
    k_buckets_serialization(const k_buckets& data): s{data}{}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
      ar & s;
    }

    std::string serialize(){
      std::stringstream stream_data;
      boost::archive::binary_oarchive oa(stream_data);
      oa << *this;
      return stream_data.str();
    }

  };
  std::size_t find_k_bucket_index(const kademlia::ID& peer_id);
  bool retrieve();
  void persist();

};
}//namespace kademlia

#endif // ROUTING_TABLE_H
