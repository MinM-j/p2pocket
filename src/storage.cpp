#include<fstream>
#include<storage.h>
std::string kademlia::storage::find_file_piece(kademlia::ID id){
  return "";
}

void kademlia::storage::store_piece(fs::path peer_path,kademlia::message msg){
  kademlia::ID hash;
  std::string content;
  msg>>hash;
  msg>>content;

  std::cout<<"hash: "<<hash<<" length: "<<content.length()<<std::endl;


  fs::path network_data_path{peer_path/kademlia::network_data_dir};
  std::cout<<"storing file "<<hash<<" in "<<network_data_path<<std::endl;

  std::ofstream fileptr{network_data_path/hash.to_string()};
  fileptr <<content;
  fileptr.close();
}
