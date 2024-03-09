#include<filesystem>
#include<stdexcept>
#include<cassert>

#include<storage.h>
#include <networking.h>
/*
//make this dir in first run
constexpr fs::path data_dir{"p2pocket/data/"};
constexprt std::size_t MAX_ALLOCATED_STORAGE = 100*1024*1024; //100 mb for now (temp)
*/

//this should start the io_context and listen for every incoming messages

namespace kademlia{
namespace network{
using namespace kademlia;
std::ostream& operator<<(std::ostream& out,const kademlia::endpoint_type& ep){
  out<< ep.first<<":"<<ep.second<<std::endl;
  return out;
}

std::ostream& operator<<(std::ostream& out , const kademlia::routing_table::k_bucket& table){
  out<<"\n["; 
  for(const auto& node: table){
    out<<node.first<<":"<<node.second.second<<", ";
  }
  out<<"]"<<std::endl;
  return out;
}
client::client(const uint16_t port, std::string id) : PORT{port}, self_id{id}, routing_table{id}{
  socket.open(asio::ip::udp::v4());
  std::cout<<"port: "<<PORT<<std::endl;
  socket.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(IPADDRESS), PORT));
}

void client::receive(){
  wait();
  io_context.run();
}

//basically a function for asynchronous receive operation
void client::wait(){
  socket.async_receive_from(asio::buffer(recv_buffer),
                            remote_endpoint,
                            boost::bind(&client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

//function to handle the receive where the message will be parsed and required action will be taken
void client::handle_receive(const system::error_code& error, size_t bytes_tranferred){
  if (error) {
    std::cout << "Receive failed : " << error.message() << std::endl;
    return;
  }

  //std::cout << "\n\nData received from(" << remote_endpoint.address() << ":" << remote_endpoint.port() << ") byte received: " << bytes_tranferred <<"\n\n";

  std::string data(recv_buffer.begin(), recv_buffer.begin()+bytes_tranferred);
  std::cout << data << std::endl;
  std::cout << "\n\n Bytes receive : " << bytes_tranferred << "\n\n"<<std::endl;


  std::stringstream stream_data;
  stream_data << data;

  message msg;

  {
    boost::archive::binary_iarchive ia(stream_data);
    ia >> msg;
  }

  endpoint_type endpoint(remote_endpoint.address().to_string(),remote_endpoint.port());

  switch(msg.type()){
    case messageType::PING:
      handle_ping_request(endpoint, msg);
      break;
    case messageType::STORE:
      handle_store_request(endpoint, msg);
      break;
    case messageType::FIND_NODE:
      handle_find_node_request(endpoint, msg);
      break;
    case messageType::FIND_VALUE:
      handle_find_value_request(endpoint, msg);
      break;

    case messageType::PING_RESPONSE:
    case messageType::STORE_RESPONSE:
    case messageType::FIND_NODE_RESPONSE:
    case messageType::FIND_VALUE_RESPONSE:
      responses.insert({msg.header.self_id, msg});
      break;

  }
  wait();
}

void client::send(const std::pair<std::string, uint16_t> endpoint,message& msg){
  asio::ip::udp::endpoint receiver_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(endpoint.first), endpoint.second);
  boost::system::error_code send_error;
  std::string data;
  {
    std::stringstream stream_data;
    boost::archive::binary_oarchive oa(stream_data);
    oa << msg;
    data = stream_data.str();
  }
  auto sent = socket.send_to(asio::buffer(data), receiver_endpoint, 0, send_error);
  std::cout << sent << " bytes sent to(" << receiver_endpoint.address() << ":" << receiver_endpoint.port() << ")\n\n";

}

client::~client(){
  socket.close();
}

void client::send_ping_request(endpoint_type endpoint){
  message request{messageType::PING, self_id};
  send(endpoint, request);
}

void client::send_store_request(endpoint_type endpoint, ID piece_hash, std::string content){
  message request{messageType::STORE, self_id};
  //TODO SERIALIZE: pair
  request<<content<<piece_hash;
  send(endpoint,request);
}
/*
void client::send_find_id_request(endpoint_type endpoint, ID node_id){
  message request{messageType::FIND_NODE, self_id};
  request<<node_id;

  send(endpoint, request);
}
*/


void client::send_find_value_request(endpoint_type endpoint, ID piece_id){
  message request{messageType::FIND_VALUE, self_id};
  request<<piece_id;

  send(endpoint, request);
}

void client::send_find_node_request(endpoint_type endpoint, ID node_id){
  message request{messageType::FIND_NODE, self_id};
  request<<node_id;

  send(endpoint, request);
}

void client::handle_ping_request(const endpoint_type endpoint,message msg){
  std::cout<<"received ping request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;

  routing_table.handle_communication(msg.header.self_id, endpoint);

  message response{messageType::PING_RESPONSE,self_id};
  send(endpoint,response);
}
//TODO: handle storage query

void client::handle_store_request(const endpoint_type endpoint,message msg){
  std::cout<<"received `store` request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;
  message response{messageType::STORE_RESPONSE,self_id};
  routing_table.handle_communication(msg.header.self_id, endpoint);

  try{
    storage::store_piece(msg);
    response<<"success";
  }catch(...){
    response<<"error";
  }
  send(endpoint,response);

}

void client::handle_find_node_request(const endpoint_type endpoint,message msg){
  std::cout<<"received 'find_node' request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;
  ID node_to_find;
  msg >> node_to_find;


  const routing_table::k_bucket  nodes = routing_table.find_node(node_to_find);
  //should be below find_node for bootstrappig purposes
  routing_table.handle_communication(msg.header.self_id, endpoint);

  message response{messageType::FIND_NODE_RESPONSE,self_id};
  response<<nodes;
  send(endpoint, response);
}


void client::handle_find_value_request(const endpoint_type endpoint,message msg){
  std::cout<<"received 'find_value' request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;
  //TODO: change find_value body
  ////send nearest nodes if value not found

  ID piece_id;
  msg>>piece_id;

  routing_table.handle_communication(msg.header.self_id, endpoint);
  std::string content =storage::find_file_piece(piece_id);

  message response{messageType::FIND_VALUE_RESPONSE,self_id};

  if(!content.empty()){
    response << content;
  }

  send(endpoint, response);
}

constexpr int replication_value = 4;
void client::store_file(ID file_hash, std::string content){
  /*
   * find all the closest nodes to the file_hash present in the routing table 
   * send find_node request to current closest nodes
   * sort the results by nearest to the file_hash
   * send store_request to the `n` nearest nodes
  */

  routing_table::k_bucket closest_nodes = routing_table.find_node(file_hash);

  //of course there will be no node with give id
  //and hence it will always return the k_bucket in which the give node would lie
  if(routing_table::is_node_in_bucket(closest_nodes, file_hash)){
    throw("Id clash. It should not have happened. Never. Incendies.");
  }


  if(closest_nodes.empty()){
    std::cout<<"error"<<std::endl;
    throw("corresp[onding k bucket is empty");
  }

  std::cout<<"hello"<<std::endl;

  nodes_tracker_type closest_nodes_tracker;
  for(auto [id, endpoint]: closest_nodes){
    closest_nodes_tracker.insert({id, peer_request_info{ endpoint, NO_ACTION, timer{}}});
  }

  //closest_nodes_tracker is passed by reference
  find_id_recursively(closest_nodes_tracker, file_hash);

  std::vector<routing_table::value_type> candidate_peers;
  for(const auto& [id, node_status]: closest_nodes_tracker){
    if(node_status.status==REQUEST_RECEIVED)
      candidate_peers.emplace_back(id, node_status.endpoint);
  }

  std::sort(candidate_peers.begin(), candidate_peers.end(),
            [file_hash](const auto& lhs, const auto& rhs){
            //comparer returns true if first argument is less than second
            //return kademlia::ID_comparer((file_hash ^ lhs.first) , (file_hash ^ rhs.first));
            return (file_hash ^ lhs.first).to_ulong()<  (file_hash ^ rhs.first).to_ulong();
            });

  //now send store request
  //TODO
}

//call this function when we don't expect `node_id`|`value_id` to be found
void client::find_id_recursively(nodes_tracker_type& closest_nodes_tracker, kademlia::ID id_to_find){

  while(1){
    std::cout<<"top "<<closest_nodes_tracker.size()<<std::endl;
    bool node_query_completed=true;
    for( auto& [id, node_status]: closest_nodes_tracker){
      switch(node_status.status){
        case NO_ACTION:
          std::cout<<"no action"<<std::endl;
          send_find_node_request(node_status.endpoint, id_to_find);
          node_status.status=REQUEST_SENT;
          node_query_completed=false;
          break;
        case REQUEST_RECEIVED:
        case REQUEST_TIMEOUT:
          std::cout<<"received or timeour"<<std::endl;
          break;
        case REQUEST_SENT:
          std::cout<<"req sent"<<std::endl;
          auto it = responses.find(id);
          std::cout<<"req sent"<<std::endl;
          std::cout<<"req sent"<<std::endl;
          if(it==responses.end()){
            std::cout<<"no responses found"<<std::endl;
            if(node_status.timer.elapsed()>TIMEOUT_LIMIT){
              node_status.status = REQUEST_TIMEOUT;
              continue;
              //TODO: find a way to remove responses of terminated request from `responses`
              //if they arrive later
            }
            //not found
            node_query_completed=false;
            continue;
          }

          std::cout<<"response received"<<std::endl;
          auto message = it->second;
          if(message.header.msg_type!=messageType::FIND_NODE_RESPONSE){
            continue;
            //TODO:
          }

          //response received from node with id `id`
          //TEMP: responses.erase(it);
          node_status.status = REQUEST_RECEIVED;
          //handle message response
          routing_table::k_bucket response_nodes;
          message >> response_nodes;

          //try inserting response nodes into nodes_tracker
          for( auto [id, endpoint]: response_nodes){
            closest_nodes_tracker.insert({id, {endpoint, NO_ACTION, timer{}}});
          }
          break;
      }
    }
    if(node_query_completed)
      break;
  }
}



void client::bootstrap(kademlia::endpoint_type boot_ep){
  //first insert id of bootstrap node into self routing table
  send_find_node_request(boot_ep, this->self_id);
  kademlia::message res;
  try{
    res=get_first_response(10);
  }
  //TODO: change to custom exception
  catch(int){
    std::cout<<"didn't receive response from bootstrap node"<<std::endl;
    exit(1);
  }

  std::cout<<"msg_type: "<<res.header.msg_type<<std::endl;

  assert((void("invalid first response") ,  res.header.msg_type==messageType::FIND_NODE_RESPONSE));

  kademlia::ID bootstrap_id=res.header.self_id;

  routing_table.insert_node(bootstrap_id, boot_ep);


  routing_table::k_bucket  kb;
  res >> kb;

  if(kb.empty())
  {
    std::cout<<"bootstrapping node didn't return any node;"<<std::endl;
    return;
  }


  nodes_tracker_type new_nodes_tracker;
  std::cout<<"nodes from bootstrapping node: "<<kb<<std::endl;

  for(auto [id, endpoint]: kb){
    std::cout<<"BBBBBB"<<std::endl;
    new_nodes_tracker.insert({id, peer_request_info{ endpoint, NO_ACTION, timer{}}});
  }
  std::cout<<"BBBBBB"<<std::endl;

  find_id_recursively(new_nodes_tracker,this->self_id);
  std::cout<<"DEBUG"<<std::endl;
  std::cout<<"total new nodes(bootstrap): "<<new_nodes_tracker.size()<<std::endl;
  std::cout<<"DEBUG"<<std::endl;


  for(const auto& [id, node_status]: new_nodes_tracker){
    //insert all nodes irrespective of status? 
    //give priority to request_received
    if(node_status.status==REQUEST_RECEIVED){
      routing_table.insert_node(id, node_status.endpoint);
    }
  }

  //this code is mess
  for(const auto& [id, node_status]: new_nodes_tracker){
    //insert all nodes irrespective of status? 
    //give priority to request_received
    if(node_status.status!=REQUEST_RECEIVED){
      routing_table.insert_node(id, node_status.endpoint);
    }
  }

}



kademlia::message client::get_first_response(double wait_time){
  using namespace std::chrono_literals;
  kademlia::timer timer;
  while(timer.elapsed()<wait_time){
    if(responses.empty())
      std::this_thread::sleep_for(20ms);
    else
      return responses.begin()->second;
  }
  std::cout<<"waited for: "<<timer.elapsed()<<"  waiting time: "<<wait_time;
  //TODO: create new exception class
  throw(1);
}

kademlia::message client::wait_response(kademlia::ID id, kademlia::messageType msg_type, double wait_time){
  using namespace std::chrono_literals;
  kademlia::timer timer;
  while(timer.elapsed()<wait_time){
    auto it=    responses.find(id);
    if(it!= responses.end()){
      kademlia::message response = it->second;
      if(response.header.msg_type==msg_type){
        responses.erase(it);
        return response;
      }
    }
    std::this_thread::sleep_for(20ms);
  }
  //TODO: create new exception class
  throw(1);
}
}//namespace network
}//namespace kademlia
