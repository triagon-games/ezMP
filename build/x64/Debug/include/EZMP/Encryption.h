#pragma once
#include "pch.h" 
#include "CommonDefinitions.h"
#ifndef AES_INSTRUCTION_SET
#include <CryptoPP/modes.h>
#include <CryptoPP/aes.h>
#include <CryptoPP/filters.h>
#endif
#ifdef AES_INSTRUCTION_SET
#include "aes_ni.h"
#include "Utils.h"
#endif

class Encryption
{
public:
	static std::string aes_decrypt(std::string plaintext, uint64_t key);
	static std::string aes_encrypt(std::string plaintext, uint64_t key);
};