#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <openssl/aes.h>
#include <openssl/evp.h>

namespace fs = std::filesystem;

extern std::string encryption_key;
extern std::string conjunction_key;

void crypto_init(fs::path peer_root_path);

std::string encrypt(const std::string& plain_text);

std::string decrypt(const std::string& cipher_text);

std::string generate_random_byte(size_t no_of_bit);
#endif
