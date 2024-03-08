#include <vector>
#include<thread>

#include<timer.h>

#include<debug.h>
#include<networking.h>

void client_debug(){

  kademlia::network::client client{2222,std::string{"111000"}};
}

void timer_debug(){
  using namespace std::chrono_literals;
  kademlia::timer timer;

  std::cout<<"thread is sleeping"<<std::endl;
  std::this_thread::sleep_for(2000ms);

  std::cout<<"elapsed_time "<<timer.elapsed()<<std::endl;

}

void routing_table_debug(){
  //event_loop();
  using endpoints=std::vector<kademlia::routing_table::node_type>;
  endpoints eps;

  eps.emplace_back("endpoint1_ip", 1111);
  eps.emplace_back("endpoint2_ip", 1112);
  eps.emplace_back("endpoint3_ip", 1113);
  //eps.emplace_back("endpoint4_ip", 1114);
  //eps.emplace_back("endpoint5_ip", 1115);
  //eps.emplace_back("endpoint6_ip", 1116);

  kademlia::ID self_id{kademlia::generate_id()};
  std::cout<<self_id<<std::endl;

  kademlia::routing_table  rt{self_id.to_string()};

  kademlia::ID lookup_node = kademlia::generate_id();

  for( auto ep: eps){
    std::cout<<"============"<<std::endl;
    auto id=  kademlia::generate_id();

    //lookup_node = id;

    std::cout<<id<<std::endl;
    rt.insert_node(id, ep);
    std::cout<<"============"<<std::endl<<std::endl;
  }

  std::cout<<"lookup node: "<<lookup_node<<std::endl;

  auto kb= rt.find_node(lookup_node);
  bool is_node_found = kademlia::routing_table::is_node_in_bucket(kb, lookup_node);
  if(is_node_found){
    std::cout<<"node found: "<< kb.front().first<<" "<<kb.front().second<<std::endl;
  }
  else{
    std::cout<<"node  not found: "<<std::endl;
    //<< kb.front().first<<" "<<kb.front().second<<std::endl;
  }

}
