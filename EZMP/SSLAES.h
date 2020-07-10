#pragma once
#include "pch.h"
#include <openssl/evp.h>

class SSLAES
{
public:
	SSLAES(unsigned char* key, int key_len);
	SSLAES();

	EVP_CIPHER_CTX* decrContext;
	EVP_CIPHER_CTX* encrContext;

	int aes_init(unsigned char* key_data, int key_data_len, EVP_CIPHER_CTX* e_ctx, EVP_CIPHER_CTX* d_ctx);

	unsigned char* aes_decrypt(EVP_CIPHER_CTX* d, unsigned char* plaintext, int* len);
	unsigned char* aes_encrypt(EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len);
};

