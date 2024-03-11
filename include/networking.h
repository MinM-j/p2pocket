
//header file to handle all the network operations (receive, send)
#ifndef NETWORKING_H
#define NETWORKING_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include <map>
#include <utility>

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <boost/bind/bind.hpp>

#include <kademlia/message.h>
#include <routing_table.h>
#include <types.h>
#include <timer.h>

using namespace boost;

//local ip of a device always constant only for testing purpose in practical should hold own ip and port it is binding
const std::string IPADDRESS = "127.0.0.1";

const double TIMEOUT_LIMIT=3; //in seconds

std::ostream& operator<<(std::ostream& out, const kademlia::routing_table::k_bucket& kb);

namespace kademlia{
namespace network{
class client{
public:
  enum peer_status{
    REQUEST_SENT,
    REQUEST_RECEIVED,
    REQUEST_TIMEOUT,
    NO_ACTION
  };

  struct peer_request_info{
    endpoint_type endpoint;
    peer_status status;
    kademlia::timer timer;
  };

  using nodes_tracker_type=std::map<ID, peer_request_info, kademlia::ID_comparer>;

  using wait_response_type = std::pair<ID, message>;
  using wait_responses_type= std::vector<wait_response_type>;

  client();
  client(const uint16_t port, std::string self_id);
  ~client();
  void initialize(const uint16_t port, std::string id,fs::path peer_root_path);

  void receive();
  void send(const std::pair<std::string, uint16_t> endpoint, kademlia::message& msg);

  void send_ping_request(endpoint_type endpoint);
  void send_store_request(endpoint_type endpoint, kademlia::ID hash, std::string content);
  void send_find_value_request(endpoint_type endpoint, kademlia::ID piece_id);
  void send_find_node_request(endpoint_type endpoint, kademlia::ID node_id);


  void find_id_recursively(nodes_tracker_type& closest_nodes_tracker, kademlia::ID id_to_find);
  void bootstrap(kademlia::endpoint_type boot_ep);

  //change name to .*_piece
  std::vector<kademlia::ID> store_file(kademlia::ID file_hash, std::string content);
  std::string retrieve_file(kademlia::ID piece_hash, const std::vector<kademlia::ID>& storing_nodes);

  wait_responses_type wait_for_responses(nodes_tracker_type& nodes_tracker,kademlia::messageType msg_type);
  kademlia::message wait_response(kademlia::ID id, kademlia::messageType msg_type, double wait_time=3);
  kademlia::message get_first_response(double wait_time=2);
private:
  void handle_receive(const system::error_code& error, size_t bytes_tranferred);
  void wait();
  //void store(std::pair<std::string,std::string> key_value);
  //void find_value(std::pair<std::string,uint16_t> endpoint, std::string node_id);
  void ping(std::pair<std::string,uint16_t> endpoint);
  void find_node(std::pair<std::string,uint16_t> endpoint, std::string node_id);

  void handle_ping_request(const endpoint_type endpoint, kademlia::message msg);
  void handle_find_node_request(const endpoint_type endpoint, kademlia::message msg);
  void handle_find_value_request(const endpoint_type endpoint, kademlia::message msg);
  void handle_store_request(const endpoint_type endpoint, kademlia::message msg);

  friend std::ostream& operator<<(std::ostream& out,const kademlia::endpoint_type& ep);
  friend std::ostream& operator<<(std::ostream& out , const kademlia::routing_table::k_bucket& table);
  //void send_find_id_request(endpoint_type endpoint, kademlia::ID node_id);

private:
  uint16_t PORT;
  kademlia::ID self_id;
  asio::io_context io_context;
  asio::ip::udp::socket socket{io_context};
  std::array<char, 100*1024> recv_buffer;
  asio::ip::udp::endpoint remote_endpoint;
  //TODO: can't have multiple responses from same node?
  std::map<kademlia::ID,kademlia::message, kademlia::ID_comparer> responses;
  kademlia::routing_table routing_table;
public:
  fs::path root_path;
};
}
}


#endif

