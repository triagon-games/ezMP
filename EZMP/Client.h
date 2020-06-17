#pragma once
#include "pch.h"
#include <time.h>
#include <chrono>
class Client
{
public:
	Client(uint64_t ClientUUID, char* password);
	~Client();
	bool exchangeKeys();
	bool sendSecureSafe();
	bool sendSafe();
	void sendSecure();
	void send();

private:
	uint64_t generatePublicSecret(uint64_t referenceMillis);
	uint64_t generatePrivateSecret(char* password);

	uint64_t publicSecret;
	uint64_t privateSecret;
};

