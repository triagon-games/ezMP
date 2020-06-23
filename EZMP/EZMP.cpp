// EZMP.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <ctime>
#include "EZMP.h"

bool Test()
{
	int TEST_NUM = 4;
	bool* TESTS = (bool*)malloc(TEST_NUM * sizeof(bool));

	TESTS[0] = sizeof(float) * 2 == sizeof(double) && sizeof(double) == 8;
	TESTS[1] = sizeof(char) == 1;

	TESTS[2] = __GAME_VER__ != 0;
	TESTS[3] = SECURE_PRIME_NUMBER != 0;

	bool pass;
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