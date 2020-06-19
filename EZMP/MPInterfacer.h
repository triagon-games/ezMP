#pragma once
#include "pch.h"
#include <time.h>
#include <chrono>
#include "Packet.h"
#include <WinSock2.h>
#include "CommonDefinitions.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class MPInterfacer
{
public:
	MPInterfacer(uint64_t ClientUUID, char* password, uint16_t sendPort, uint8_t* address, uint16_t recvPort);
	~MPInterfacer();

	void sendPacket(Packet pkt);

	Packet recvPacket();

private:
	bool sendSecureReliable(Packet pkt);
	bool sendReliable(Packet pkt);
	void sendSecure(Packet pkt);
	void sendSimple(Packet pkt);

	uint64_t generatePublicSecret(uint64_t referenceMillis);
	uint64_t generatePrivateSecret(char* password);

	uint64_t publicSecret;
	uint64_t privateSecret;
};