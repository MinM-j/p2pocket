#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <openssl/aes.h>
#include <openssl/evp.h>


extern std::string encryption_key;
extern std::string conjuction_key;

void crypto_init();

std::string encrypt(const std::string& plain_text);

std::string decrypt(const std::string& cipher_text);

#endif
