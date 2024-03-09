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


kademlia::network::client client;
const std::size_t PIECE_SIZE{100};

void init_node(int argc , char* argv[]){
  if(argc<3){
    std::cout<<" expected: port"<<std::endl;
    exit(1);
  }

  std::string peer_name=argv[1];
  int port = std::atoi(argv[2]);
  std::cout<<"peer: "<<peer_name<<" port: "<<port<<std::endl;

  const fs::path peer_root_path(kademlia::project_path/peer_name);

  create_init_directories(peer_root_path);
  auto id = create_new_id(peer_root_path);

  const kademlia::endpoint_type boot_node{"127.0.0.1",8848};

  client.initialize(port,id,peer_root_path);

  std::thread recv_thread{[&](){client.receive();}};

  client.bootstrap(boot_node);

  //client.find_node(boot_node, "11100011" );

  try{
    event_loop(peer_name);
  }catch(int){
    //recv_thread.join();
    std::cout<<"quitng"<<std::endl;
  }
}



std::string create_new_id(fs::path peer_root_path){
  fs::path id_path{ peer_root_path/kademlia::id_file};

  if(fs::exists(id_path)){
    std::ifstream fptr{id_path};
    std::string id;
    fptr>>id;
    std::cout<<"read id from 'id.dat': "<<id<<std::endl;
    return id;
  }

  const std::string id = kademlia::generate_id().to_string();
  std::ofstream fptr{id_path};
  fptr << id;
  std::cout<<"generated new id: "<<id<<std::endl;

  return id;
}

void create_init_directories(fs::path peer_root_path){
  const fs::path peer_network_files_path{peer_root_path/kademlia::network_data_dir};
  const fs::path peer_filesystem{peer_root_path/kademlia::network_fs_dir};

  std::cout<<"creating necessary files and dirs"<<std::endl;

  std::error_code ec;
  bool success;

  success = fs::create_directories(peer_root_path,ec);
  success = fs::create_directories(peer_network_files_path,ec);
  success = fs::create_directories(peer_filesystem,ec);
}
void event_loop(std::string peer_name){
  std::string input;

  using enum input_command_type;
  std::initializer_list<std::tuple<input_command_type, std::string, int>> commands = {
    {HELP, "help", 1},
    {STORE, "store", 2},
    {RETRIEVE, "retrieve", 2},
    {LS, "ls", 1},
    {PWD, "pwd", 1},
    {QUIT, "quit", 1},
  };
  while(1){
    std::cout<<"p2pocket["<<peer_name<<"]: ";
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
    case QUIT:
      execute_quit();
      break;
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

void execute_quit(){
  throw(1);
}


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
  std::cout<<"Iterating dir: "<<client.root_path<<std::endl;
  std::string command{"tree "};
  command += client.root_path;
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
  //TODO: IMP: iterate through all args(paths)
  //TODO: IMP: encryption
  //TODO: IMP: set storage limit
  //TODO: IMP: filter self id so that not to send req to self
  //TODO: persist ID(done) and routing table
  //TODO: broadcast ip change
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
    kademlia::ID id{key};
    //client.store_file(id, value);
    std::cout<<key<<" "<<value.size()<<std::endl;
    client.store_file(id,value);
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
    char c;
    c=hash[0];
    int intval = (c >= 'a') ? (c - 'a' + 10) : (c - '0');


    intval=intval<<4;

    std::bitset<NO_OF_BIT> id(intval);
    c=hash[1];
    intval =  (c >= 'a') ? (c - 'a' + 10) : (c - '0');

    id|=intval;


    //return hash;
    return id.to_string();
    //TEMP TODO: remove substr
  };

  for(int i = 0; i<no_of_pieces - 1; i++){
    std::string data(PIECE_SIZE, '\0');
    fileptr.read(&data[0], PIECE_SIZE);
    std::string hash=get_hash(data);
    pieces_content.emplace_back(hash,data);
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
  fs::path metadata_path{client.root_path/filename};

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
