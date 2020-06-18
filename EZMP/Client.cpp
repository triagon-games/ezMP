#include "Client.h"
uint64_t uuid;

uint64_t count_digit(uint64_t number)
{
	return uint64_t(log10((double)number) + 1);
}

Client::Client(uint64_t ClientUUID, char* password, uint16_t port, uint8_t* address)
{
	uuid = ClientUUID;
	publicSecret = generatePublicSecret(NULL);

	//TODO: SERGEI FINISH SENDING PACKET
}

Client::~Client()
{
}

bool Client::exchangeKeys()
{
	return false;
}

bool Client::sendSecureReliable(Packet pkt)
{
	return false;
}

bool Client::sendReliable(Packet pkt)
{
	return false;
}

void Client::sendSecure(Packet pkt)
{
}

void Client::send(Packet pkt)
{
}

void Client::sendPacketRaw(Packet pkt)
{
}

uint64_t Client::generatePublicSecret(uint64_t referenceMillis)
{
	uint64_t milliseconds = 0;
	if (referenceMillis != NULL)
	{
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		milliseconds = _byteswap_uint64(ms.count());
		delete &ms, sizeof(std::chrono::milliseconds);
	}
	else
	{
		milliseconds = referenceMillis;
	}
	uint8_t* m_ms = (uint8_t*)&milliseconds;

	uint32_t salt = (m_ms[0] ^ m_ms[1]) << 24 | (m_ms[2] ^ m_ms[3]) << 16 | (m_ms[4] ^ m_ms[5]) << 8 | (m_ms[6] ^ m_ms[7]);

	uint64_t maxMinNumber = 0;

	for (int i = 0; i < count_digit(UINT64_MAX); i++)
	{
		maxMinNumber += (uint64_t)(10, i);
	}
	srand((uint32_t)(milliseconds%UINT32_MAX));
	uint64_t publicKey = (rand() + maxMinNumber) % UINT64_MAX;
	publicKey = publicKey ^ uuid;

	publicKey = (((uint64_t)(((uint32_t*)&publicKey)[0] ^ salt)) << 32) | (((uint32_t*)&publicKey)[1] ^ salt);

	size_t keyHash = std::hash<long>{}(publicKey);

	delete& milliseconds;
	return keyHash;
}

uint64_t Client::generatePrivateSecret(char* password)
{
	return uint64_t();
}
