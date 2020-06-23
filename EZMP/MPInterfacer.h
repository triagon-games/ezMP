#pragma once
#include "pch.h"
#include <time.h>
#include <chrono>
#include "Packet.h"
#include <WinSock2.h>
#include "CommonDefinitions.h"
#include <ws2tcpip.h>
#include <thread>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class MPInterfacer
{
private: 
	void (*m_ReceiveCallback)(Packet);
	void (*m_LatencyCallback)(uint16_t);

	uint64_t power(uint64_t a, uint64_t b, uint64_t P)
	{
		if (b == 1)
			return a;

		else
			return (((uint64_t)std::pow(a, b)) % P);
	}

public:
	MPInterfacer(uint64_t ClientUUID, std::string password, uint16_t sendPort, uint8_t* address, uint16_t recvPort);
	~MPInterfacer();

	void sendPacket(Packet* pkt, bool retry = false);

	Packet recvPacket();

	void attachReceiveCallback(void (*func)(Packet));
	void attachLatencyCallback(void (*func)(uint16_t));

private:
	std::thread ListenerThread;
	std::thread ACKManagerThread;

	std::vector<Packet*> ACKBuffer;
	int ACKBufferLength = 0;

	bool awaitPacket();
	Packet encryptPacket(Packet pkt);

	void startHandshake();

	uint64_t generatePublicSecret(uint64_t referenceMillis);
	uint64_t generatePrivateSecret(std::string password);
	void onHandshakeReceive(uint64_t secret, uint32_t exchangeNum, uint64_t referenceTime);

	uint64_t publicKey = 0;
	uint64_t privateKey = 0;
protected: uint64_t sharedSecret = 0;

	void ListenerFunction();
	void ACKManager();
};