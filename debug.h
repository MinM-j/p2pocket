#include <interface.h>
#include <routing_table.h>
#include<id.h>
#include<filesystem>

namespace fs=std::filesystem;

void timer_debug();
void routing_table_debug();
void client_debug();
//required parameters: name port 
void init(int argc , char* argv[]);
void init(std::string peer_name, int port);


