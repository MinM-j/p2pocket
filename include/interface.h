#include<iostream>
#include<string>
#include<vector>
#include<filesystem>

namespace fs=std::filesystem;

enum class input_command_type{
  HELP,
  STORE,
  RETRIEVE,
  LS,
  FILE_STATUS,
  PWD
};


using args_type = std::vector<std::string>;
using splitted_entries_type=std::vector<std::pair<std::string, std::string>>;

void event_loop();
args_type parse_input(const std::string& input);
void handle_input(input_command_type command,const args_type& args);
void execute_help();
void execute_store_command(const args_type& args);
void execute_pwd();

void store_directory(fs::path dir_path);
void store_file(fs::path file_path);
void execute_list_files();
splitted_entries_type split_and_hash_file(fs::path file);
void store_file(fs::path file_path);
void create_metadata_file(const splitted_entries_type& splitted_entries,const fs::path& filename);

/*void handle_get_storage_info(const std::size_t requested_storage){
  using it = fs::recursive_directory_iterator;

  std::size_t total_used_size{};

  for(const auto& dir_entry: fs::recursive_directory_iterator(data_dir)){
    if(fs::is_directory(dir_entry))
      continue;
    total_occupied_size = dir_entry.file_size();
  }
  const std::size_t available_storage= MAX_ALLOCATED_STORAGE - total_used_size;

  if(available_storage>requested_storage){
    //note if two peer concurrently request for resouces before actually storing file, 
    //then both will get ok response but only one of them will be able to store if the 
    //free storage is just enough for one file
    //solution: create a reserved file with random content of file size of requested_storage 
    //and delete the file when the node actually stores the content
    //OR just reject when the latter node actually wants to store the file IDK? 
    //This is to do after the mid defense

    //RETURN OK STATUS  and storage details too?
  }
  else{
    //RETURN NO STORAGE STATUS 
  }
}


void network::client::execute_store_command(std::string file_or_dir_path){
  //not though about absolute or relative path
  //since no idea what will be the cwd used by fs::exists in case of relative path?
  // make the path relative to the cwd?
  if(!fs::exists(file_or_dir_path)){
    //std::cout<<"invalid file or directory path: "<<file_or_dir_path<<std::endl;
    throw std::invalid_argument("invalid file or directory or path: "+file_or_dir_path);// may not work without c_str()
  }

  if(fs::directory(file_or_dir_path)){
    //do this later
    return;
  }

  client.store_file();


  }
}
*/

//void network::client::handle_input(command, args);

//void trim_string(std::string& str_to_trim);
