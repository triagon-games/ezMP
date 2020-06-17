#pragma once
#include "pch.h"
class Client
{
public:
	Client(uint64_t ClientUUID);
	~Client();
	bool exchangeKeys();
	bool sendSecureSafe();
	bool sendSafe();
	void sendSecure();
	void send();

private:
	uint64_t generatePublicKey();
	uint64_t generateSecretKey();
};

