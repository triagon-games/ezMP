#include "Encryption.h"
#ifndef AES_INSTRUCTION_SET
std::string Encryption::aes_decrypt(std::string ciphertext, uint64_t key)
{
	CryptoPP::byte ppKey[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];

	memcpy(ppKey, &key, sizeof(key));
	memcpy(ppKey + 8, &key, sizeof(key));

	memset(iv, 0x0f, CryptoPP::AES::BLOCKSIZE);

	CryptoPP::AES::Decryption aesDecryption(ppKey, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	std::string decryptedtext;

	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
	stfDecryptor.MessageEnd();

	return decryptedtext;
}

std::string Encryption::aes_encrypt(std::string plaintext, uint64_t key)
{
	CryptoPP::byte ppKey[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];

	memcpy(ppKey, &key, sizeof(key));
	memcpy(ppKey + 8, &key, sizeof(key));

	memset(iv, 0x0f, CryptoPP::AES::BLOCKSIZE);

	CryptoPP::AES::Encryption aesEncryption(ppKey, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	std::string ciphertext;

	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
	stfEncryptor.MessageEnd();

	return ciphertext;
}
#endif
#ifdef AES_INSTRUCTION_SET
std::string Encryption::aes_decrypt(std::string ciphertext, uint64_t key)
{
	uint8_t* key128 = (uint8_t*)malloc(16);
	if (key128 == nullptr) throw std::exception("key128 malloc failed line: ", __LINE__ - 1);
	memcpy(key128, &key, sizeof(uint64_t));
	memcpy(&(key128[8]), &key, sizeof(uint64_t));
	__m128i keyschedule[20];
	aes128_load_key(key128, keyschedule);
	std::string compute_plaintext;

	uint8_t* cipherBlock = (uint8_t*)malloc(16);
	uint8_t* plainBlock = (uint8_t*)malloc(16);
	if (cipherBlock == nullptr) throw std::exception("cipherBlock malloc failed line: ", __LINE__ - 2);
	if (plainBlock == nullptr) throw std::exception("plainBlock malloc failed line: ", __LINE__ - 2);
	for (int i = 0; i < ceil(ciphertext.size() / 16.0f); i++)
	{
		ZeroMemory(cipherBlock, 16);
		ZeroMemory(plainBlock, 16);
		memcpy(cipherBlock, &ciphertext.c_str()[i * 16], min(ciphertext.size()-16*i, 16));
		aes128_dec(keyschedule, cipherBlock, plainBlock);
		compute_plaintext += Utils::stringFromBytes(plainBlock, 16);
	}

	return compute_plaintext;
}

std::string Encryption::aes_encrypt(std::string plaintext, uint64_t key)
{
	uint8_t* key128 = (uint8_t*)malloc(16);
	if (key128 == nullptr) throw std::exception("key128 malloc failed line: ", __LINE__ - 1);
	memcpy(key128, &key, sizeof(uint64_t));
	memcpy(&(key128[8]), &key, sizeof(uint64_t));
	__m128i keyschedule[20];
	aes128_load_key(key128, keyschedule);
	std::string compute_ciphertext;

	uint8_t cipherBlock[16];
	uint8_t plainBlock[16]; 
	if (cipherBlock == nullptr) throw std::exception("cipherBlock malloc failed line: ", __LINE__ - 2);
	if (plainBlock == nullptr) throw std::exception("plainBlock malloc failed line: ", __LINE__ - 2);
	for (int i = 0; i < ceil(plaintext.size() / 16.0f); i++)
	{
		ZeroMemory(cipherBlock, 16);
		ZeroMemory(plainBlock, 16);
		memcpy(plainBlock, &plaintext.c_str()[i * 16], min(plaintext.size() - 16 * i, 16));
		aes128_enc(keyschedule, plainBlock, cipherBlock);
		compute_ciphertext += Utils::stringFromBytes(cipherBlock, 16);
	}


	return compute_ciphertext;
}
#endif