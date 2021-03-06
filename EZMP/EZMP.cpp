// EZMP.cpp : Defines the functions for the static library.
//
#include "EZMP.h"
#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <ctime>
#include <cassert>

bool Test()
{
	int TEST_NUM = 6;
	bool* TESTS = (bool*)malloc(TEST_NUM * sizeof(bool));
	if (TESTS == nullptr) throw std::runtime_error("malloc failed");

	TESTS[0] = sizeof(float) * 2 == sizeof(double) && sizeof(double) == 8;
	TESTS[1] = sizeof(char) == 1;

	TESTS[2] = __GAME_VER__ != 0;
	TESTS[3] = SECURE_PRIME_NUMBER != 0;

	std::string encrypted = Encryption::aes_encrypt("this is a test string", 227);
	std::string decrypted = Encryption::aes_decrypt(encrypted, 227);

	TESTS[4] = strcmp(decrypted.c_str(), "this is a test string") == 0;

	uint32_t publicKey = 876876;
	uint32_t aKey = 123123;
	uint32_t bKey = 456456;

	uint64_t aSide = MPInterfacer::generateRuledKey(MPInterfacer::generateRuledKey(publicKey, bKey, SECURE_PRIME_NUMBER), aKey, SECURE_PRIME_NUMBER);
	uint64_t bSide = MPInterfacer::generateRuledKey(MPInterfacer::generateRuledKey(publicKey, aKey, SECURE_PRIME_NUMBER), bKey, SECURE_PRIME_NUMBER);

	TESTS[5] = aSide == bSide;

	assert(sizeof(float) * 2 == sizeof(double) && sizeof(double) == 8);
	assert(sizeof(char) == 1);
	assert(__GAME_VER__ != 0);
	assert(SECURE_PRIME_NUMBER != 0);
	assert(strcmp(decrypted.c_str(), "this is a test string") == 0);
	assert(aSide == bSide);

	bool pass = true;
	for (int i = 0; i < TEST_NUM; i++)
	{
		if (!TESTS[i]) pass = false;
	}
	return pass;
}

int EZMP::Init()
{
	if (!Test()) throw std::runtime_error("SYSTEM TESTS FAILED");
	return 0;
}