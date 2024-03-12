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
  out<< ep.first<<":"<<ep.second;
  return out;
}

std::istream& operator<<(std::istream& in, kademlia::endpoint_type& ep){
  std::getline(in, ep.first, ':');
  in>>ep.second;

  std::cout<<ep<<std::endl;
  return in;
}

std::ostream& operator<<(std::ostream& out , const kademlia::routing_table::k_bucket& table){
  out<<"\n["; 
  for(const auto& node: table){
    out<<node.first<<":"<<node.second.second<<", ";
  }
  out<<"]"<<std::endl;
  return out;
}

client::client()=default;
client::client(const uint16_t port, std::string id):
  PORT{port},
  self_id{id},
  routing_table{id}{
  socket.open(asio::ip::udp::v4());
  socket.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(IPADDRESS), PORT));
}

void client::initialize(const uint16_t port, std::string id,fs::path peer_root_path,bool log){
  this->self_id=ID{id};
  this->PORT=port;
  this->routing_table=kademlia::routing_table{id};
  this->root_path=peer_root_path;
  this->log_byte_info=log;

  socket.open(asio::ip::udp::v4());
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


  std::string data(recv_buffer.begin(), recv_buffer.begin()+bytes_tranferred);


  std::stringstream stream_data;
  stream_data << data;

  message msg;

  {
    boost::archive::binary_iarchive ia(stream_data);
    ia >> msg;
  }

  endpoint_type endpoint(remote_endpoint.address().to_string(),remote_endpoint.port());
  //if(log_byte_info || endpoint.second == kademlia::boot_port){
  if(log_byte_info ){
    std::cout <<"\n================================================================"<<std::endl;
    std::cout << "\nBytes received:" << bytes_tranferred<<std::endl;
    std::cout << msg.header.msg_type<< " from "<<remote_endpoint<<std::endl;
  }


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
  if(sent<0){
    std::cout<<"error(send): "<<send_error.message()<<std::endl;
  }

  if(log_byte_info ){
    //if(log_byte_info || endpoint.second == kademlia::boot_port){
    std::cout << sent << " bytes sent to(" << receiver_endpoint.address() << ":" << receiver_endpoint.port() << ")\n";
    std::cout<<"msg_type: "<<msg.header.msg_type<<std::endl<<std::endl;
  }
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
  //std::cout<<"received ping request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;

  routing_table.handle_communication(msg.header.self_id, endpoint);

  message response{messageType::PING_RESPONSE,self_id};
  send(endpoint,response);
}
//TODO: handle storage query

void client::handle_store_request(const endpoint_type endpoint,message msg){
  //std::cout<<"received `store` request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;
  message response{messageType::STORE_RESPONSE,self_id};

  routing_table.handle_communication(msg.header.self_id, endpoint);

  try{
    storage::store_piece(this->root_path,msg);
    response<<"success";
  }catch(...){
    response<<"error";
  }
  send(endpoint,response);

}

void client::handle_find_node_request(const endpoint_type endpoint,message msg){
  //std::cout<<"received 'find_node' request from "<<endpoint <<std::endl;
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
  //std::cout<<"received 'find_value' request from "<<endpoint<<"["<<msg.header.self_id<<"]"<<std::endl;
  //TODO: change find_value body
  ////send nearest nodes if value not found

  ID piece_id;
  msg>>piece_id;

  routing_table.handle_communication(msg.header.self_id, endpoint);
  std::string content =storage::find_file_piece(this->root_path,piece_id);

  message response{messageType::FIND_VALUE_RESPONSE,self_id};

  //if not found send empty response
  if(!content.empty()){
    response << content;
  }

  send(endpoint, response);
}

bool client::off_log_byte(){
  if(log_byte_info)
    std::cout<<"LOG OFF"<<std::endl;
  bool res=log_byte_info;
  log_byte_info=false;
  return res;
}

void client::set_log_byte(bool log_byte){
  this->log_byte_info=log_byte;
  if(log_byte_info)
    std::cout<<"LOG ON"<<std::endl;
}

std::vector<kademlia::routing_table::value_type> client::store_file(ID file_hash, std::string content){
  std::cout<<"\n\n";
  std::cout<<"responses in map:"<<std::endl;
  //TEMP BUG FIX
  responses.clear();
  //std::cout<<"id: "<<id<<" "<<msg.header.msg_type<<std::endl;
  /*
   * find all the closest nodes to the file_hash present in the routing table 
   * send find_node request to current closest nodes
   * sort the results by nearest to the file_hash
   * send store_request to the `n` nearest nodes
  */
  auto curr_log_byte=this->off_log_byte();

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


  nodes_tracker_type closest_nodes_tracker;
  for(auto [id, endpoint]: closest_nodes){
    closest_nodes_tracker.insert({id, peer_request_info{ endpoint, NO_ACTION, timer{}}});
  }

  //closest_nodes_tracker is passed by reference
  find_id_recursively(closest_nodes_tracker, file_hash);

  std::vector<routing_table::value_type> candidate_peers;

  for(const auto& [id, node_status]: closest_nodes_tracker){
    if(node_status.status==REQUEST_RECEIVED && id != this->self_id)
      candidate_peers.emplace_back(id, node_status.endpoint);
  }
  //TODO: 

  std::sort(candidate_peers.begin(), candidate_peers.end(),
            [file_hash](const auto& lhs, const auto& rhs){
            //comparer returns true if first argument is less than second
            //return kademlia::ID_comparer((file_hash ^ lhs.first) , (file_hash ^ rhs.first));
            return (file_hash ^ lhs.first).to_ulong()<  (file_hash ^ rhs.first).to_ulong();
            });

  //std::cout<<"total nodes found for piece("<<file_hash<<"): "<<candidate_peers.size()<<std::endl;
  nodes_tracker_type storing_nodes_tracker;

  auto no_of_nodes=std::min(kademlia::replica_count,static_cast<int>(candidate_peers.size()));

  std::cout<<"STORING PIECE "<<file_hash<<" in peers:"<<std::endl;
  std::for_each(candidate_peers.begin(), std::next(candidate_peers.begin(),no_of_nodes),
                [&](auto candidate){
                auto candidate_id = candidate.first;
                auto endpoint = candidate.second;

                if(endpoint.second == kademlia::boot_port){
                }
                std::cout<<endpoint<<", ";
                send_store_request(endpoint, file_hash, content);
                storing_nodes_tracker.insert({candidate_id,{endpoint, REQUEST_SENT, timer{}}});
                });

  wait_responses_type storing_node_responses = wait_for_responses(storing_nodes_tracker,messageType::STORE_RESPONSE);
  //view response and do accordingly TODO: IMP

  std::vector<kademlia::routing_table::value_type> storing_nodes;
  for(auto [storing_node, response]: storing_node_responses){
    std::string msg; //success or error
    response>>msg;
    //std::cout<<"msg: "<<msg<<std::endl;
    if(msg!="success"){
      std::cout<<"failed to store "<<file_hash << " in node "<<storing_node.second<<std::endl;
    }
    storing_nodes.push_back(storing_node);
    //std::cout<<"id: "<<id<<" message: "<<msg<<std::endl;
  }

  set_log_byte(curr_log_byte);
  return storing_nodes;
}

client::wait_responses_type client::wait_for_responses(nodes_tracker_type& nodes_tracker , kademlia::messageType msg_type){
  wait_responses_type responses_vec;

  while(1){
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms);
    bool node_query_completed = true;
    for( auto& [id, node_status]: nodes_tracker){
      switch(node_status.status){
        case REQUEST_RECEIVED:
        case REQUEST_TIMEOUT:
          break;
        case NO_ACTION:
          std::cout<<"(error): all nodes should have been sent request"<<std::endl;
          throw(1);
          break;
        case REQUEST_SENT:
          auto it = responses.find(id);
          if(it==responses.end()){
            if(node_status.timer.elapsed()>TIMEOUT_LIMIT){
              std::cout<<"timing out "<< id <<":"<<node_status.endpoint<<std::endl;
              node_status.status = REQUEST_TIMEOUT;
              continue;
              //TODO: find a way to remove responses of terminated request from `responses`
              //if they arrive later
            }
            //not found
            node_query_completed=false;
            continue;
          }

          //std::cout<<"found response from "<<node_status.endpoint<<std::endl;
          auto message = it->second;
          if(message.header.msg_type!=msg_type){
            continue;
            //TODO:
          }

          std::cout<<"port "<<node_status.endpoint.second <<std::endl;
          responses.erase(it);
          //std::cout<<"msg type: "<<message.header.msg_type<<std::endl;
          responses_vec.emplace_back(std::make_pair(id,node_status.endpoint), message);
          break;
      }
    }
    if(node_query_completed)
      break;
  }
  return responses_vec;

}

//call this function when we don't expect `node_id`|`value_id` to be found
void client::find_id_recursively(nodes_tracker_type& closest_nodes_tracker, kademlia::ID id_to_find, bool find_endpoint){
  while(1){
    bool node_query_completed=true;
    for( auto& [id, node_status]: closest_nodes_tracker){
      switch(node_status.status){
        case NO_ACTION:
          if(id != this->self_id)
            send_find_node_request(node_status.endpoint, id_to_find);
          else
            std::cout<<"ignored self id temp"<<std::endl;
          node_status.status=REQUEST_SENT;
          node_query_completed=false;
          break;
        case REQUEST_RECEIVED:
        case REQUEST_TIMEOUT:
          break;
        case REQUEST_SENT:
          auto it = responses.find(id);
          if(it==responses.end()){
            if(node_status.timer.elapsed()>TIMEOUT_LIMIT){
              std::cout<<"timing out "<< id <<":"<<node_status.endpoint<<std::endl;
              node_status.status = REQUEST_TIMEOUT;
              continue;
              //TODO: find a way to remove responses of terminated request from `responses`
              //if they arrive later
            }
            //not found
            node_query_completed=false;
            continue;
          }

          auto message = it->second;
          responses.erase(it);
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
            if(id != this->self_id)
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

  //to prevent resendig request oto bootstrap node
  new_nodes_tracker.insert({bootstrap_id, peer_request_info{ boot_ep, REQUEST_RECEIVED, timer{}}});
  for(auto [id, endpoint]: kb){
    new_nodes_tracker.insert({id, peer_request_info{ endpoint, NO_ACTION, timer{}}});
  }

  find_id_recursively(new_nodes_tracker,this->self_id);

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

  std::cout<<"find_nodes process completed"<<std::endl;
  std::cout<<"total new nodes(bootstrap): "<<new_nodes_tracker.size()<<std::endl;
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

std::string client::retrieve_file(kademlia::ID piece_hash, const std::vector<kademlia::routing_table::value_type>& storing_nodes){
  //bug fix
  responses.clear();

  nodes_tracker_type storing_nodes_tracker;

  //std::cout<<"TEST"<<std::endl;

  for(const auto& [storing_node_id , endpoint]: storing_nodes){
    send_find_value_request(endpoint, piece_hash);
    storing_nodes_tracker.insert({storing_node_id,{endpoint, REQUEST_SENT, timer{}}});
  }

  wait_responses_type storing_node_responses = wait_for_responses(storing_nodes_tracker,messageType::FIND_VALUE_RESPONSE);

  if(!storing_node_responses.empty()){
    for(auto [storing_node, response]: storing_node_responses){
      std::string content;
      response>>content;

      std::cout<<"RECEIVED CONTENT from " <<storing_node.second<< " SIZE: "<<content.size()<<std::endl;
      if(content.empty()){
        continue;
      }
      else 
      //check hash from content 
      return content;
    }
  }
  std::cout<<"couldn't receive valid content from any storing node for piece "<<piece_hash<<std::endl;
  throw(1);
}
}//namespace network
}//namespace kademlia
