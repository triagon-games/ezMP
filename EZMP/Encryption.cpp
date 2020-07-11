#include "Encryption.h"

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
