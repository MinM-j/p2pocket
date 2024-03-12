#include<string>
#include<id.h>
#include<kademlia/message.h>

namespace kademlia{
namespace storage{
void store_piece(kademlia::message msg);
void store_piece(fs::path peer_path,kademlia::message msg);
std::string find_file_piece(fs::path peer_root_path,kademlia::ID id);
}
}
