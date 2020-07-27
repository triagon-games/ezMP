#pragma once
#include "Packet.h"
#include "Utils.h"
class NTPacket
{
public:

	EZMP_DLL NTPacket(Packet* p);
	EZMP_DLL NTPacket(uint16_t id, Packet* p);
	EZMP_DLL NTPacket(uint16_t id, bool multicastable, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum);
	EZMP_DLL uint16_t getPacketEnumeration();
	EZMP_DLL Packet* getBasePacket();

	EZMP_DLL template<typename T> uint32_t appendVariable(T var, uint16_t enumeration)
	{
		m_Packet->appendData((uint8_t*)&enumeration, 2, PACKET_VARIABLE_NAME);
		return m_Packet->appendData(var);
	}

	EZMP_DLL float getFloatByEnumeration(uint16_t id);
	EZMP_DLL double getDoubleByEnumeration(uint16_t id);
	EZMP_DLL uint8_t getByteByEnumeration(uint16_t id);
	EZMP_DLL uint16_t getShortByEnumeration(uint16_t id);
	EZMP_DLL uint32_t getIntByEnumeration(uint16_t id);
	EZMP_DLL uint64_t getLongByEnumeration(uint16_t id);
	EZMP_DLL std::string getStringByEnumeration(uint16_t id);

private:
	Packet* m_Packet;
	uint16_t m_packetID;
};