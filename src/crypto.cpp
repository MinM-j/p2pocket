#include <crypto.h>
#include<config.h>

namespace fs = std::filesystem;

std::string encryption_key;
std::string conjuction_key;

void crypto_init(fs::path peer_root_path){
	///* TODO: write a file name*/

  fs::path encryption_key_file_path{peer_root_path/kademlia::encryption_key_file};
	if(std::filesystem::exists(encryption_key_file_path)){
		std::ifstream file{encryption_key_file_path};
		std::stringstream buffer;
		file.close();
		buffer << file.rdbuf();
		std::string file_data{buffer.str()};
		encryption_key.assign(file_data.substr(0, file_data.find('\n')));
		conjuction_key.assign(file_data.substr(file_data.find('\n')+1));
	}
	else{
		std::ofstream file{encryption_key_file_path};
		///* TODO: create 256 bit random for encryption_key and 128 bit random for conjuction_key*/
		file << encryption_key << "\n" << conjuction_key;
		file.close();
	}
}

std::string encrypt(const std::string& plain_text){
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	std::string cipher_text;
	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(encryption_key.c_str()), reinterpret_cast<const unsigned char*>(conjuction_key.c_str()));
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
	EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(encryption_key.c_str()), reinterpret_cast<const unsigned char*>(conjuction_key.c_str()));
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
