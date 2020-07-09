#pragma once
#include "pch.h"
#include <time.h>
#include <chrono>
#include "Packet.h"
#include "Utils.h"
#include <WinSock2.h>
#include <winerror.h>
#include "CommonDefinitions.h"
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include "PortForwardEngine.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class MPInterfacer
{
private: 
	void (*m_ReceiveCallback)(Packet);
	void (*m_LatencyCallback)(uint16_t);

public:
	MPInterfacer(uint64_t ClientUUID, uint16_t Port, uint8_t* address, bool isServer, std::string Pass);
	~MPInterfacer();

	void sendPacket(Packet* pkt, bool retry = false);

	Packet recvPacket();

	void attachReceiveCallback(void (*func)(Packet));
	void attachLatencyCallback(void (*func)(uint16_t));

	static uint64_t generateRuledKey(uint32_t a, uint32_t b, uint32_t P)
	{
		return ((uint64_t)a * (uint64_t)b) % P;
	}

private:
	std::thread ListenerThread;
	std::thread ACKManagerThread;

	void HolePunch();

	std::vector<Packet*> ACKBuffer;
	std::vector<Utils::Endpoint> ServersideEndpoints;
	int ACKBufferLength = 0;

	bool awaitPacket();
	Packet encryptPacket(Packet pkt);

	void startHandshake();

	uint32_t generatePublicKey(uint64_t referenceMillis);
	uint32_t generatePrivateKey(std::string password);
	void onHandshakeReceive(uint32_t secret, uint32_t exchangeNum, uint64_t referenceTime);

	uint32_t publicKey = 0;
	uint32_t privateKey = 0;
protected: uint64_t sharedSecret = 0;

	void ListenerFunction();
	void ACKManager();
};