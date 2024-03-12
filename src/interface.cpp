#include<map>
#include<sstream>
#include<iomanip>
#include<stdexcept>
#include<cstdlib>
#include<fstream>
#include<cmath>
#include<sstream>
#include<ios>

#include<interface.h>
#include<sha1.h>
#include<networking.h>
#include<crypto.h>

kademlia::network::client client;
const std::size_t PIECE_SIZE{100};

fs::path peer_root_path;
fs::path network_files_path; //stored files of network
fs::path network_filesystem; //metadata
fs::path retrieved_data_path; //retrieved data

void init_node(int argc , char* argv[]){
  if(argc<3){
    std::cout<<" expected: port"<<std::endl;
    exit(1);
  }

  std::string peer_name=argv[1];
  int port = std::atoi(argv[2]);
  std::cout<<"peer: "<<peer_name<<" port: "<<port<<std::endl;

  peer_root_path = kademlia::project_path/peer_name;

  create_init_directories(peer_root_path);
  crypto_init(peer_root_path);
  auto id = create_new_id(peer_root_path);

  const kademlia::endpoint_type boot_node{"127.0.0.1",kademlia::boot_port};

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
  network_files_path = peer_root_path/kademlia::network_data_dir;
  network_filesystem = peer_root_path/kademlia::network_fs_dir;
  retrieved_data_path = peer_root_path/kademlia::data_dir;


  std::cout<<"creating necessary files and dirs"<<std::endl;

  std::error_code ec;
  bool success;

  success = fs::create_directories(peer_root_path,ec);
  success = fs::create_directories(network_files_path,ec);
  success = fs::create_directories(network_filesystem,ec);
  success = fs::create_directories(retrieved_data_path,ec);

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

    bool command_handled=false;
    for(const auto& [command, curr_command_string, required_args_count]: commands){
      if(input_command_str == curr_command_string){
        command_handled=true;
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
    if(!command_handled)
      execute_help();
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
      execute_retrieve(args);
      break;
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
  //std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"pwd"<<std::endl;
  std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"retrieve"<<"[file | dir]"<<std::endl;
  std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"status"<<"[file | dir]"<<std::endl;
  //std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"retrieve"<<"[file | dir]"<<std::endl;
  //std::cout<<"\t"<<std::left<<std::setw(command_characters)<<"retrieve"<<"[file | dir]"<<std::endl;

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
  //TODO: IMP: frequent crash automatically 
  //TODO: IMP: cannot close and start node again
  //TODO: IMP: directory name in each recursion (store directory)
  //======================
  //TODO: IMP: iterate through all args(paths)
  //TODO: IMP: handle ID clash 
  //TODO: IMP: set storage limit
  //TODO: IMP: filter self id so that not to send req to self
  //TODO: persist ID(done) and routing table
  //TODO: broadcast ip change
  //======================
  //TODO: IMP: change ID size(done)
  //TODO: IMP: bootstrapping node not storing file(done)
  //TODO: IMP: encryption(done)

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

  splitted_entries_type splitted_entries = encrypt_split_and_hash_file(file_path);

  std::cout<<"storing file: "<<file_path<<std::endl;
  std::cout<<"total piece: "<<splitted_entries.size()<<std::endl;

  std::vector<std::pair<kademlia::ID, std::vector<kademlia::routing_table::value_type>>> all_storing_nodes;

  for(const auto& [key,value] : splitted_entries){
    kademlia::ID id{key};
    //client.store_file(id, value);
    //std::cout<<key<<" size: "<<value.size()<<std::endl;
    auto response = client.store_file(id,value);
    std::cout<<"storingnodes: "<<response.size();
    //store_responses.emplace_back(key, store_responses)
    all_storing_nodes.emplace_back(key, response);
  }

  std::cout<<"=========================================="<<std::endl;

  //TO create a metadata file we need all the piece hash, and corresponding storing nodes.
  std::ofstream  metadata_file{network_filesystem / file_path.filename()};

  for(const auto& [piece_hash, storing_nodes] : all_storing_nodes){
    metadata_file<<piece_hash;
    for( auto [node_id, node_endpoint]: storing_nodes ){
      metadata_file<<" "<<node_id<<" "<<node_endpoint;
    }
    metadata_file<<"\n";
  }

  metadata_file.close();
}

splitted_entries_type encrypt_split_and_hash_file(fs::path file){
  std::ifstream fileptr{file, std::ios::binary | std::ios::ate};
  fileptr.seekg(0,std::ios::beg);


  std::stringstream sstream;
  sstream<<fileptr.rdbuf();

  std::string encrypted_content = encrypt(sstream.str());

  auto file_size = encrypted_content.size();

  //std::cout<<"ORIGINAL size: "<<fs::file_size(file)<<std::endl;
  //std::cout<<"ENCRYPTED size: "<<encrypted_content.size()<<std::endl;

  sstream.str(""); //emptying sstream
  sstream<<encrypted_content;

  //sstream.seekg(0, std::ios::end);
  //auto file_size = fileptr.tellg();
  //fileptr.seekg(0,std::ios::beg);

  auto no_of_pieces = std::ceil(file_size/float(PIECE_SIZE));

  //std::pair<hash, content>
  std::vector<std::pair<std::string, std::string>> pieces_content;
  pieces_content.reserve(no_of_pieces);

  /*
   * the library porvides hash in hex string
   * need to change it into bitset
   */
  auto get_hash= [](const std::string& data){
    SHA1 hashing;
    hashing.update(data);
    std::string actual_hash=hashing.final();//this is in hex form

    kademlia::ID id;
    for(int i=0; i<NO_OF_BIT / 4 ;i++){\
      char c=actual_hash[i]; //first nibble for i = 0
      int intval = (c >= 'a') ? (c - 'a' + 10) : (c - '0');
      //intval=intval<<4;
      id <<= 4;
      id |= intval;
    }

    std::cout<<"actual_hash "<<actual_hash<<std::endl
      <<" truncated_hash: "<<id<<std::endl;
    return id.to_string();
    //TEMP TODO: remove substr
  };

  for(int i = 0; i<no_of_pieces - 1; i++){
    std::string data(PIECE_SIZE, '\0');
    sstream.read(&data[0], PIECE_SIZE);
    std::string hash=get_hash(data);
    pieces_content.emplace_back(hash,data);
  }

  //read last piece
  auto last_piece_size = file_size % PIECE_SIZE;
  std::string data(last_piece_size, '\0');
  sstream.read(&data[0], last_piece_size);
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

void execute_retrieve(const args_type& args){

  const auto file_or_dir_name = args[1];
  const auto& file_or_dir_path=network_filesystem/file_or_dir_name;

  //not though about absolute or relative path
  //since no idea what will be the cwd used by fs::exists in case of relative path?
  // make the path relative to the cwd?

  if(!fs::exists(file_or_dir_path)){
    throw std::invalid_argument("invalid file or directory path: "+file_or_dir_name);
  }

  if(fs::is_directory(file_or_dir_path)){
    std::cout<<"retrieving directory: "<<file_or_dir_path <<std::endl;
    retrieve_directory(file_or_dir_path);
    //do this later
    return;
  }
  else
  retrieve_file(file_or_dir_path);
}

void retrieve_directory(fs::path dir_path){
  std::cout<<"iterating directory: "<< dir_path <<std::endl;
  for (const auto& dir_entry : fs::directory_iterator(dir_path)) {
    if (fs::is_directory(dir_entry)) {
      // If the entry is a directory, recursively process it
      retrieve_directory(dir_entry);
    } else if (fs::is_regular_file(dir_entry)) {
      // If the entry is a regular file, call the storefile function
      retrieve_file(dir_entry);
    }
    else{
      //not a regular file or directory
      std::cout<<"handle this error";
      throw("handle this");
    }
  }
}


void retrieve_file(fs::path file_path){
  std::cout<<"retrieving file: "<<file_path<<std::endl;

  std::ifstream metadata_file{file_path};
  std::string line;
  std::vector<std::string> pieces_metadata;
  while(std::getline(metadata_file, line)){
    pieces_metadata.push_back(line);
  }

  std::cout<<"total pieces: "<<pieces_metadata.size()<<std::endl;
  std::cout<<"piece hashes: \n";

  std::vector<std::pair<kademlia::ID, std::vector<kademlia::routing_table::value_type>>> all_storing_nodes;
  for(const auto& piece_metadata: pieces_metadata){
    std::vector<kademlia::routing_table::value_type> storing_nodes;
    std::stringstream sstream;
    sstream<< piece_metadata;
    std::string id;

    sstream>>id;//piece_id
    kademlia::ID piece_id{id};

    kademlia::endpoint_type ep;
    //std::cout<<"storing nodes: "<<std::endl;
    while(sstream>>id && sstream>>ep){
      storing_nodes.emplace_back(id, ep);
      //std::cout<<id<<" " <<ep<<std::endl;
    }

    all_storing_nodes.emplace_back(piece_id, storing_nodes);
    //std::cout<<e<<std::endl;
  }

  std::stringstream content_stream;
  try{
    for(const auto& [piece_hash, storing_nodes] : all_storing_nodes){
      std::string content = client.retrieve_file(piece_hash, storing_nodes);
      content_stream<<content;
    }
  }
  catch(int){
    return;
  }

  std::cout<<"content size: "<<content_stream.str().size()<<std::endl;

  std::string decrypted_content = decrypt(content_stream.str());

  fs::path dest_path{retrieved_data_path/file_path.filename()};
  std::ofstream fptr{dest_path};
  fptr<<decrypted_content;
  std::cout<<"retrieved data in file: "<<dest_path<<std::endl;
  //content_stream << content;
}


/*
    std::shared_ptr<std::map<std::string,std::string>> piece_contents = std::make_shared<std::map<std::string,std::string>>();

    for(const std::string hash: piece_hashes){

    auto on_load = [=]( std::error_code const& error
    , k::session::data_type const& data ) mutable
    {
    std::cout<<std::endl<<std::endl;
    std::cout<<"captured hash "<<hash<<std::endl;
    if ( error )
    {
    std::cout<<"err"<<std::endl;
    std::cerr << "Failed to load \"" << hash << "\" piece of file" << filename<< std::endl;
    }
    //error handle
    else
    {
    //std::cout<<"received response "<<std::endl;
    std::string const content{ data.begin(), data.end() };

    std::cout<<"map size before emplace"<<piece_contents->size()<<std::endl;
    std::cout<<"emplacing: "<<hash<<" "<<content.size()<<std::endl;

    piece_contents->emplace(hash, content);

    std::cout<<"map contents: "<<std::endl;
    for(auto p : *piece_contents)
    std::cout<<p.first<<" "<<p.second.size()<<std::endl;
    std::cout<<std::endl;
    if(piece_contents->size()==piece_hashes.size()){
    std::cout<<"file piece collected"<<std::endl;
    std::stringstream filestream;
    for(const auto piece_hash: piece_hashes){
    auto value_iterator = piece_contents->find(piece_hash);
    filestream << value_iterator->second;
    }
    std::ofstream fileptr{p2pocket_retrieve_dir/filename};//todo: dir
    fileptr<<filestream.rdbuf();
    std::cout<<"The retrieved file is: "<<p2pocket_retrieve_dir/filename<<std::endl;
    }
    }
    };
    session.async_load( hash, std::move( on_load ) );
    }
    */
