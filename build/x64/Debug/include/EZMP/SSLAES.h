#pragma once
#include "pch.h" 

class SSLAES
{
public:
	SSLAES(unsigned char* key, int key_len);
	SSLAES();

	int aes_init(unsigned char* key_data, int key_data_len);

	unsigned char* aes_decrypt(unsigned char* plaintext, int* len);
	unsigned char* aes_encrypt(unsigned char* plaintext, int* len);
};