#include "MPInterfacer.h"
uint64_t uuid;

uint64_t count_digit(uint64_t number)
{
	return uint64_t(log10((double)number) + 1);
}

MPInterfacer::MPInterfacer(uint64_t ClientUUID, char* password, uint16_t port, uint8_t* address)
{
	uuid = ClientUUID;
	publicSecret = generatePublicSecret(NULL);

	//TODO: SERGEI FINISH SENDING PACKET
}

MPInterfacer::~MPInterfacer()
{
}

bool MPInterfacer::sendSecureReliable(Packet pkt)
{
	return false;
}

bool MPInterfacer::sendReliable(Packet pkt)
{
	return false;
}

void MPInterfacer::sendSecure(Packet pkt)
{
}

void MPInterfacer::send(Packet pkt)
{
}

void MPInterfacer::sendPacket(Packet pkt)
{
}

uint64_t MPInterfacer::generatePublicSecret(uint64_t referenceMillis)
{
	return std::hash<long>{}(referenceMillis);
}

uint64_t MPInterfacer::generatePrivateSecret(char* password)
{
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	uint64_t milliseconds = _byteswap_uint64(ms.count());
	delete& ms, sizeof(std::chrono::milliseconds);

	uint8_t* m_ms = (uint8_t*)&milliseconds;

	uint32_t salt = (m_ms[0] ^ m_ms[1]) << 24 | (m_ms[2] ^ m_ms[3]) << 16 | (m_ms[4] ^ m_ms[5]) << 8 | (m_ms[6] ^ m_ms[7]);

	uint64_t maxMinNumber = 0;

	for (int i = 0; i < count_digit(UINT64_MAX); i++)
	{
		maxMinNumber += (uint64_t)(10, i);
	}
	srand((uint32_t)(milliseconds % UINT32_MAX));
	uint64_t privateKey = (rand() + maxMinNumber) % UINT64_MAX;
	privateKey = privateKey ^ uuid;

	privateKey = (((uint64_t)(((uint32_t*)&privateKey)[0] ^ salt)) << 32) | (((uint32_t*)&privateKey)[1] ^ salt);

	size_t passHash = std::hash<char*>{}(password);
	size_t keyHash = std::hash<long>{}(privateKey);

	delete& milliseconds;
	return keyHash ^ passHash;
}