#include <networking.h>
#include<filesystem>
#include<stdexcept>

/*
//make this dir in first run
constexpr fs::path data_dir{"p2pocket/data/"};
constexprt std::size_t MAX_ALLOCATED_STORAGE = 100*1024*1024; //100 mb for now (temp)
*/

//namespace fs = std::filesystem;

//this should start the io_context and listen for every incoming messages
kademlia::network::client::client(const uint16_t port) : PORT{port}{
	socket.open(asio::ip::udp::v4());
	socket.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(IPADDRESS), PORT));
}

void kademlia::network::client::receive(){
	wait();
	io_context.run();
}

//basically a function for asynchronous receive operation
void kademlia::network::client::wait(){
	socket.async_receive_from(asio::buffer(recv_buffer),
			remote_endpoint,
			boost::bind(&client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

//function to handle the receive where the message will be parsed and required action will be taken
void kademlia::network::client::handle_receive(const system::error_code& error, size_t bytes_tranferred){
	if (error) {
		std::cout << "Receive failed : " << error.message() << std::endl;
		return;
	}

	std::cout << "\n\nData received from(" << remote_endpoint.address() << ":" << remote_endpoint.port() << ") byte received: " << bytes_tranferred <<"\n\n";

	std::string data(recv_buffer.begin(), recv_buffer.begin()+bytes_tranferred);
	std::cout << data << std::endl;
	std::cout << "\n\n Bytes receive : " << bytes_tranferred << "\n\n";

	std::stringstream stream_data;
	stream_data << data;
	
	kademlia::message msg;

	{
		boost::archive::binary_iarchive ia(stream_data);
		ia >> msg;
	}

	if (msg.type() == kademlia::messageType::PING) {
		std::cout << "\nIt is ping";
	}

	int a; float d; char b; std::string e;

	msg >> b >> d >> a;

	std::cout << a << "\n" << d << "\n" << b << "\n" << std::endl;

	wait();
}

void kademlia::network::client::send(const std::pair<std::string, uint16_t> endpoint,const kademlia::message& msg){
	asio::ip::udp::endpoint receiver_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(endpoint.first), endpoint.second);
	boost::system::error_code send_error;
	std::string data;
	{
		std::stringstream stream_data;
		boost::archive::binary_oarchive oa(stream_data);
		oa << msg;
		data = stream_data.str();
	}
	auto sent = socket.send_to(asio::buffer(data), receiver_endpoint, 0, send_error);
	std::cout << sent << " bytes sent to(" << receiver_endpoint.address() << ":" << receiver_endpoint.port() << ")\n\n";

}

void kademlia::network::client::ping(std::pair<std::string,uint16_t> endpoint){
	message ping{messageType::PING};
	send(endpoint, ping);
}

void kademlia::network::client::find_node(std::pair<std::string,uint16_t> endpoint, std::string node_id){

}

kademlia::network::client::~client(){
	socket.close();
}
/*
void handle_get_storage_info(const std::size_t requested_storage){
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

void network::client::event_loop(){
  std::string input;


  using enum input_command_type;
  std::map<input_command_type, std::string> command_entries{
        {HELP,"help"},
        {STORE,"store"},
        {RETRIVE,"retrieve"},
        {LS,"ls"},
        {FILE_STATUS,"file_status"}
  };

  while(1){
    std::cout<<"p2pocket: ";
    std::cin>>input;

    for(auto [command, command_string]: command_entries){
      if(input.starts_with(command_string)){
        command_string.erase(0, command_string.length()+1); //+1 for white space(single) , use trim instead
        try{
        }
        catch(const std::invalid_argument& error){
          std::cout<<"error(invalid_args): "<<error<<std::endl;
        }
        break;
      }
    }
  }
}

void network::client::handle_input(command, args){
  using enum input_command_type;
  switch(command, args){
    case HELP:
      //print help
      break;
    case STORE:
      //"store  file_path or directory_path"
      client.execute_store_command(args);
      break;
    case RETRIVE:
      break;
    case LS:
      break;
    case FILE_STATU:
      break;
  };
}

void trim_string(std::string& str_to_trim){
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
