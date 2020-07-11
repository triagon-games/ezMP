#pragma once
#include "pch.h" 
#include <CryptoPP/modes.h>
#include <CryptoPP/aes.h>
#include <CryptoPP/filters.h>
#include "aes_ni.h"

class Encryption
{
public:
	static std::string aes_decrypt(std::string plaintext, uint64_t key);
	static std::string aes_encrypt(std::string plaintext, uint64_t key);
};