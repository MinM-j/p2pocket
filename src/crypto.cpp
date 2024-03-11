#include <crypto.h>

void crypto_init(){
	///* TODO: write a file name*/
	const std::string file_name = ""; // file containing key and conjuction text
	if(std::filesystem::exists(file_name)){
		std::ifstream file{file_name};
		std::stringstream buffer;
		file.close();
		buffer << file.rdbuf();
		std::string file_data{buffer.str()};
		key.assign(file_data.substr(0, file_data.find('\n')));
		iv.assign(file_data.substr(file_data.find('\n')+1));
	}
	else{
		std::ofstream file{file_name};
		///* TODO: create 256 bit random for key and 128 bit random for iv*/
		file << key << "\n" << iv;
		file.close();
	}
}

std::string encrypt(const std::string& plain_text){
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	std::string cipher_text;
	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), reinterpret_cast<const unsigned char*>(iv.c_str()));
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
	EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(key.c_str()), reinterpret_cast<const unsigned char*>(iv.c_str()));
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
