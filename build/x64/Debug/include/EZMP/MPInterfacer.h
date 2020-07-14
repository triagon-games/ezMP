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
#include "Encryption.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class MPInterfacer
{
private: 
	void (*m_ReceiveCallback)(Packet);
	void (*m_LatencyCallback)(uint16_t);

public:
	struct Configuration
	{
	public:
		uint32_t ACKRetryCooldown = ACK_RETRY;
		uint32_t ACKTimeoutTime = ACK_TIMEOUT;
		uint32_t GameVersion = 1;
		uint32_t SecurePrimeNumber = SECURE_PRIME_NUMBER;
		std::string PublicStunServer = PUBLIC_STUN_SERVER;
		uint32_t ListenInterval = 0;
		uint32_t ACKInterval = 0;

		Configuration() {}
	};

	Configuration Config;

	MPInterfacer(uint64_t ClientUUID, uint16_t Port, uint8_t* address, bool isServer, std::string Pass, Configuration cfg = Configuration());
	~MPInterfacer();

	void sendPacket(Packet* pkt, bool retry = false);
	void sendPacket(Packet* pkt, bool retry, Utils::Endpoint endpoint);

	std::vector<Utils::Endpoint> ServersideEndpoints;
	void multicastPacket(Packet* pkt, std::vector<Utils::Endpoint> group);

	Packet recvPacket();

	void attachReceiveCallback(void (*func)(Packet));
	void attachLatencyCallback(void (*func)(uint16_t));

	static uint64_t generateRuledKey(uint32_t a, uint32_t b, uint32_t P)
	{
		return ((uint64_t)a * (uint64_t)b) % P;
	}

	bool isServer;

private:
	std::thread ListenerThread;
	std::thread ACKManagerThread;

	void HolePunch();

	std::vector<Packet*> ACKBuffer;
	int ACKBufferLength = 0;

	Packet encryptPacket(Packet pkt, uint64_t key);
	Packet decryptPacket(Packet pkt, uint64_t key);

	void startHandshake();

	uint32_t generatePublicKey(uint64_t referenceMillis);
	uint32_t generatePrivateKey(std::string password);
	void onHandshakeReceive(uint32_t secret, uint32_t exchangeNum, uint64_t referenceTime, Utils::Endpoint source);

	uint32_t publicKey = 0;
	uint32_t privateKey = 0;

	Utils::PortTranslation localPortMapping;
	Utils::Endpoint whatAmI;

protected: uint64_t sharedSecret = 0;

	void ListenerFunction();
	void ACKManager();
};