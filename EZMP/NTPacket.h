#pragma once
#include "Packet.h"
#include "Nameof.hpp"
#include "Utils.h"
class NTPacket
{
public:

	NTPacket(Packet* p);
	NTPacket(uint16_t id, Packet* p);
	NTPacket(uint16_t id, bool multicastable, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum);
	uint16_t getPacketEnumeration();
	Packet* getBasePacket();

	template<typename T>
	uint32_t appendVariable(T var, uint16_t enumeration);

	template<typename T>
	T getVariableByEnumeration(uint16_t id);

private:
	Packet* m_Packet;
	uint16_t m_packetID;
};