#include "SSLAES.h"

SSLAES::SSLAES(unsigned char* key, int key_len)
{
	aes_init(key, key_len, encrContext, decrContext);
}

SSLAES::SSLAES() {}

int SSLAES::aes_init(unsigned char* key_data, int key_data_len, EVP_CIPHER_CTX* e_ctx, EVP_CIPHER_CTX* d_ctx)
{
    int i, nrounds = 5;
    unsigned char key[32], iv[32];
    unsigned int salt[] = { 12345, 54321 };
    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), (unsigned char*)&salt, key_data, key_data_len, nrounds, key, iv);
    if (i != 32) {
        printf("Key size is %d bits - should be 256 bits\n", i);
        return -1;
    }

    EVP_CIPHER_CTX_init(e_ctx);
    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
}

unsigned char* SSLAES::aes_decrypt(EVP_CIPHER_CTX* d, unsigned char* plaintext, int* len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char* encplaintext = (unsigned char*)malloc(p_len);

    EVP_DecryptInit_ex(d, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(d, plaintext, &p_len, plaintext, *len);
    EVP_DecryptFinal_ex(d, plaintext + p_len, &f_len);

    *len = p_len + f_len;
    return encplaintext;
}

unsigned char* SSLAES::aes_encrypt(EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len)
{
    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = *len + 16, f_len = 0;
    unsigned char* ciphertext = (unsigned char*)malloc(c_len);

    /* allows reusing of 'e' for multiple encryption cycles */
    EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

    /* update ciphertext, c_len is filled with the length of ciphertext generated,
      *len is the size of plaintext in bytes */
    EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

    /* update ciphertext with the final remaining bytes */
    EVP_EncryptFinal_ex(e, ciphertext + c_len, &f_len);

    *len = c_len + f_len;
    return ciphertext;
}
