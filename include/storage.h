#include<string>
#include<id.h>
#include<kademlia/message.h>

namespace kademlia{
namespace storage{
	std::string find_file_piece(kademlia::ID id);
	void store_piece(kademlia::message msg);
}
}
/*
#include<iostream>
#include<filesystem>
#include<system_error>
#include<fstream>
#include<vector>

namespace fs = std::filesystem;

constexpr std::size_t PIECE_SIZE = 100; //in bytes
constexpr std::size_t NODE_REPLICA_SIZE=2;
namespace storage{

  void store_file(fs::path file){
    std::error_code ec;

    if(ec){
      std::cout<<"error reading file size of "<<file<<std::endl;
    }

    std::ifstream fileptr{file, std::ios::binary | std::ios::ate};

    auto file_size = fileptr.tellg();
    fileptr.seekg(0);

    auto no_of_pieces = std::ceil(file_size/float(PIECE_SIZE));

    
    //std::pair<hash, content>
    std::vector<std::pair<std::string, std::string>> pieces_content;
    pieces_content.reserve(no_of_pieces);

    //encrypt the file before splitting it into pieces

    auto get_hash= [](const std::string& data){
      SHA1 hashing;
      hashing.update(data);
      return hashing.final();
    };

    std::cout<<"no of piece "<< no_of_pieces<<std::endl;
    for(int i = 0; i<no_of_pieces - 1; i++){
      std::string data(PIECE_SIZE, '\0');
      fileptr.read(&data[0], PIECE_SIZE);
      pieces_content.emplace_back(get_hash(data).c_str(),data.c_str());
    }

    //read last piece
    auto last_piece_size = file_size % PIECE_SIZE;
    std::string data(last_piece_size, '\0');
    fileptr.read(&data[0], last_piece_size);
    pieces_content.emplace_back(get_hash(data).c_str(),data.c_str());

    std::for_each(pieces_content.begin(), pieces_content.end(),
        []( auto data){
          std::cout<<data.first<<std::endl;
          std::cout<<data.second<<std::endl<<std::endl;
    });

    //now find nodes to store the file
    auto no_of_nodes = NODE_REPLICA_SIZE * no_of_pieces;



  

  }
};
*/
