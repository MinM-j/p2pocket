#include<map>
#include<sstream>
#include<iomanip>
#include<stdexcept>
#include<cstdlib>
#include<fstream>
#include<cmath>

#include<interface.h>
#include<sha1.h>
#include<networking.h>

kademlia::network::client client{2222, std::string{"1110001100"}};

void event_loop(){

  std::string input;

  using enum input_command_type;
  std::initializer_list<std::tuple<input_command_type, std::string, int>> commands = {
    {HELP, "help", 1},
    {STORE, "store", 2},
    {RETRIEVE, "retrieve", 2},
    {LS, "ls", 1},
    {PWD, "pwd", 1},
  };
  while(1){
    std::cout<<"p2pocket: ";
    //std::cin>>input;
    std::getline(std::cin, input);
    if(input.empty())
      continue;

    const auto args = parse_input(input);

    auto& input_command_str=args[0];

    for(const auto& [command, curr_command_string, required_args_count]: commands){
      if(input_command_str == curr_command_string){
        try{
          if(args.size() < required_args_count)
            throw std::invalid_argument{"incomplete arguments."};
          handle_input(command, args);
        }
        catch(const std::invalid_argument& error){
          //try...catch necessry?
          std::cout<<"error(invalid_args): "<<error.what()<<std::endl;
          //TODO: print help for each command separately
          //print_help(command);
        }
        break;
      }
    }
    std::cout<<std::endl;
  }
}

args_type parse_input(const std::string& input){
  std::istringstream input_stream{input};
  args_type args;
  std::string word;

  while(input_stream>>word){
    args.push_back(word);
  }
  return args;
}

void handle_input(input_command_type command, const args_type& args){
  using enum input_command_type;
  switch(command){
    case HELP:
      execute_help();
      //print help
      break;
    case STORE:
      execute_store_command(args);
      break;
    case LS:
      execute_list_files();
      break;
    case PWD:
      execute_pwd();
      break;
    //"store  file_path or directory_path"
    //client.execute_store_command(args);
    //break;
    case RETRIEVE:
    //execute_retrieve();
    //break;
    case FILE_STATUS:
      //execute_file_status();
      //break;
      std::cout<<"unknown command: "<<args[0]<<std::endl;
  };
}
//TODO: make all the directories in first initiliazation
const std::string p2pocket_root_dir{"~/p2pocket/context_1/"};
//data
//filesystem
const std::size_t PIECE_SIZE{100};

void execute_pwd(){
  std::cout<<fs::current_path()<<std::endl;
}

void execute_help(){
  int command_characters{7};
  std::cout<<"commands: "<<std::endl;
  std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"help"<<std::endl;
  std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"store"<<"[file | dir]"<<std::endl;
  std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"ls"<<std::endl;
  std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"pwd"<<std::endl;
  std::cout<<std::endl;
}

void execute_list_files(){
  std::cout<<"Iterating dir: "<<p2pocket_root_dir<<std::endl;
  std::string command{"tree "};
  command += p2pocket_root_dir;
  //TODO: write our own ls code?
  std::system(command.c_str());
  //  for(const auto& dir_entry: fs::directory_iterator(p2pocket_root_dir)){
  //    if(dir_entry.is_directory()){
  //      std::cout<<"(d)";
  //    }
  //    std::cout<<dir_entry.path().filename()<<"\n";
  //  }
}

/*
 * The path provided from the commandline will be relative to the current_working_directory..
 */

void execute_store_command(const args_type& args){
  //TODO: iterate through all args(paths)
  const auto& file_or_dir_path=args[1];
  //not though about absolute or relative path
  //since no idea what will be the cwd used by fs::exists in case of relative path?
  // make the path relative to the cwd?
  if(!fs::exists(file_or_dir_path)){
    throw std::invalid_argument("invalid file or directory path: "+file_or_dir_path);// may not work without c_str()
  }

  if(fs::is_directory(file_or_dir_path)){
    std::cout<<"storing directory: "<<file_or_dir_path <<std::endl;
    store_directory(file_or_dir_path);
    //do this later
    return;
  }
  else
  store_file(file_or_dir_path);
}

void store_directory(fs::path dir_path){
  std::cout<<"iterating directory: "<< dir_path <<std::endl;
  for (const auto& dir_entry : fs::directory_iterator(dir_path)) {
    if (fs::is_directory(dir_entry)) {
      // If the entry is a directory, recursively process it
      store_directory(dir_entry);
    } else if (fs::is_regular_file(dir_entry)) {
      // If the entry is a regular file, call the storefile function
      store_file(dir_entry);
    }
    else{
      //not a regular file or directory
      std::cout<<"handle this error";
      throw("handle this");
    }
  }
}

void store_file(fs::path file_path){
  std::cout<<"storing file: "<<file_path<<std::endl;

  splitted_entries_type splitted_entries = split_and_hash_file(file_path);

  std::cout<<"total piece: "<<splitted_entries.size()<<std::endl;
  for(const auto& [key,value] : splitted_entries){
    //const auto key = content_entry.first;
    //const auto value = content_entry.second;

    client.store_file(kademlia::ID{key}, value);
    std::cout<<key<<" "<<value.size()<<std::endl;
  }

  //TODO: store the file


  //create_metadata_file(splitted_entries, file_path);
}

splitted_entries_type split_and_hash_file(fs::path file){
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
    std::string hash=hashing.final();//this is in hex form
    std::stringstream ss;

    //u<<hash<<std::endl;
    char c;
    c=hash[0];
    int intval = (c >= 'a') ? (c - 'a' + 10) : (c - '0');

    //u<<"c: "<<c<<std::endl;
    //u<<intval<<std::endl;

    intval=intval<<4;
    //u<<intval<<std::endl;

    std::bitset<NO_OF_BIT> id(intval);
    c=hash[1];
    intval =  (c >= 'a') ? (c - 'a' + 10) : (c - '0');

    //u<<"c: "<<c<<std::endl;
    //u<<intval<<std::endl;

    id|=intval;

    //std::cout<<id.to_string()<<std::endl;



    //std::cout<<std::endl;

    //return hash;
    return id.to_string();
    //TEMP TODO: remove substr
  };

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

  //std::for_each(pieces_content.begin(), pieces_content.end(),
  //[]( auto data){
  //std::cout<<data.first<<std::endl;
  //std::cout<<data.second<<std::endl<<std::endl;
  //});

  return pieces_content;
}
void create_metadata_file(const splitted_entries_type& splitted_entries,const fs::path& filename){
  //TODO: store storing_nodes id
  fs::path metadata_path{p2pocket_root_dir/filename};

  std::ofstream fptr{p2pocket_root_dir/fs::path{"hhhh.hhh"}};
  fptr<<"fuck you";
  fptr.close();
  std::ofstream metadata_file{metadata_path};

  if(metadata_file.is_open())
    std::cout<<"File is open"<<std::endl;
  else
    std::cout<<"File is not open"<<std::endl;

  std::cout<<"metadata file: "<<metadata_path<<std::endl;
  for(const auto& splitted_entry: splitted_entries){
    metadata_file << splitted_entry.first<<"\n";
  }
  metadata_file.close();

  std::ifstream imetadata_file{metadata_path};

  std::string line;
  std::cout<<fs::file_size(metadata_path)<<std::endl;
  while (std::getline(imetadata_file, line)) {
    std::cout << line << std::endl;
  }
  std::cout<<std::endl;

}
