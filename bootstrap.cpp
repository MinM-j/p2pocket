#include<debug.h>
#include<interface.h>
#include<networking.h>

int main(int argc,  char* argv[]){
  std::string peer_name="bootstrap";
  const fs::path peer_root_path(kademlia::project_path/peer_name);

  create_init_directories(peer_root_path);
  auto id = create_new_id(peer_root_path);

  int port;
  if(argc==1)
    port=kademlia::boot_port;
  else
    port = std::atoi(argv[1]);

  kademlia::network::client client{};
  client.initialize(port,id, peer_root_path, true);//log -> true

  std::cout<<"bootstrap node is on on port: "<<port;

  client.receive();

  return 0;
}
