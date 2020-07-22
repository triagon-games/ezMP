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

	template<typename T>
	EZMP_DLL uint32_t appendVariable(T var, uint16_t enumeration);

	template<typename T>
	EZMP_DLL T getVariableByEnumeration(uint16_t id);

private:
	Packet* m_Packet;
	uint16_t m_packetID;
};