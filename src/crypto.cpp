#include <crypto.h>
#include<config.h>


std::string encryption_key;
std::string conjunction_key;
const size_t encryption_key_size = 32; //byte
const size_t conjunction_key_size = 16; //byte

void crypto_init(fs::path peer_root_path){
  fs::path encryption_key_file_path{peer_root_path/kademlia::encryption_key_file};
  if(std::filesystem::exists(encryption_key_file_path)){
    std::cout<<"reading encryption keys from file "<<encryption_key_file_path<<std::endl;

    std::ifstream file{encryption_key_file_path}; std::stringstream buffer;

    buffer << file.rdbuf();
    file.close();
    std::string file_data{buffer.str()};

    encryption_key.assign(file_data.substr(0, encryption_key_size));
    conjunction_key.assign(file_data.substr(encryption_key_size + 1));
  }
  else{

    fs::create_directories(peer_root_path);
    std::cout<<" creating file for encrption keys: "<<encryption_key_file_path<<std::endl;
    std::ofstream file{encryption_key_file_path};
    if(!file.is_open()){
      std::cout<<"file not open"<<std::endl;
    }
    encryption_key = generate_random_byte(encryption_key_size);
    conjunction_key = generate_random_byte(conjunction_key_size);

    //std::cout<<"encryption key "<<encryption_key.size()<<std::endl;
    //std::cout<<"conjunction key "<<conjunction_key.size()<<std::endl;

    file << encryption_key << conjunction_key;
    file.close();
  }
}

std::string encrypt(const std::string& plain_text){
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  std::string cipher_text;
  EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(encryption_key.c_str()), reinterpret_cast<const unsigned char*>(conjunction_key.c_str()));
  int len{};
  int cipher_text_len{};
  cipher_text.resize(plain_text.length() + AES_BLOCK_SIZE);
  EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&cipher_text[0]), &len, reinterpret_cast<const unsigned char*>(plain_text.c_str()), plain_text.length());
  cipher_text_len = len;
  EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&cipher_text[0]) + len, &len);
  cipher_text_len += len;
  cipher_text.resize(cipher_text_len);
  EVP_CIPHER_CTX_free(ctx);
  return cipher_text;
}

std::string decrypt(const std::string& cipher_text){
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  std::string plain_text;
  EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(encryption_key.c_str()), reinterpret_cast<const unsigned char*>(conjunction_key.c_str()));
  int len{};
  int plain_text_len{};
  plain_text.resize(cipher_text.length());
  EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&plain_text[0]), &len, reinterpret_cast<const unsigned char*>(cipher_text.c_str()), cipher_text.length());
  plain_text_len = len;
  EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&plain_text[0]) + len, &len);
  plain_text_len += len;
  plain_text.resize(plain_text_len);
  EVP_CIPHER_CTX_free(ctx);
  return plain_text;
}

std::string generate_random_byte(size_t no_of_byte){ 
  std::random_device random_device;
  std::mt19937 generator;
  generator.seed(random_device());

  constexpr int generator_bit = 8; //size of std::mt19937()??


  std::stringstream sstream;

  for(int i = 0; i< no_of_byte;i++){
    auto random = generator();

    unsigned int mask = (1 << generator_bit) - 1;

    uint8_t byte = random & mask;
    sstream << static_cast<char>(byte);

  }
  return sstream.str();
}

