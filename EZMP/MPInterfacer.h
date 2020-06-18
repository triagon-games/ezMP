#pragma once
#include "pch.h"
#include <time.h>
#include <chrono>
#include "Packet.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class MPInterfacer
{
public:
	MPInterfacer(uint64_t ClientUUID, char* password, uint16_t port, uint8_t* address);
	~MPInterfacer();
	bool sendSecureReliable(Packet pkt);
	bool sendReliable(Packet pkt);
	void sendSecure(Packet pkt);
	void send(Packet pkt);

private:
	void sendPacket(Packet pkt);

	uint64_t generatePublicSecret(uint64_t referenceMillis);
	uint64_t generatePrivateSecret(char* password);

	uint64_t publicSecret;
	uint64_t privateSecret;
};