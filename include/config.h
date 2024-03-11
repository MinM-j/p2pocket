#ifndef CONFIG_H
#define CONFIG_H


#include<iostream>
#include<string>
#include<bitset>
#include<random>
#include<list>
#include<array>

#include<filesystem>

//for now keep it here
inline constexpr int NO_OF_BIT = 8 ;
inline constexpr int CURRENT_BUCKET_SIZE=4;

namespace kademlia{

inline const std::filesystem::path project_path{ "./p2pocket_data"};

inline const std::string network_data_dir{ "network_data"};
inline const std::string network_fs_dir{ "network_fs"};
inline const std::string data_dir{ "data"};

inline const std::string id_file{"id.dat"};
inline const std::string encryption_key_file{"encryption_key.dat"};

inline const int replica_count=2;
}

#endif //CONFIG_H
