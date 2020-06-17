#include "pch.h"
#include "Client.h"
#include <time.h>

uint64_t uuid;

Client::Client(uint64_t ClientUUID)
{
	uuid = ClientUUID;
}

Client::~Client()
{
}

bool Client::exchangeKeys()
{
	return false;
}

bool Client::sendSecureSafe()
{
	return false;
}

bool Client::sendSafe()
{
	return false;
}

void Client::sendSecure()
{
}

void Client::send()
{
}

uint64_t Client::generatePublicKey()
{
	//time();


	return uint64_t();
}

uint64_t Client::generateSecretKey()
{

	return uint64_t();
}
