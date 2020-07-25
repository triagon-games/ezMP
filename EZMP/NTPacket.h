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
		switch (typeid(var).name())
		{
		case typeid(float).name():
			break;
		case typeid(double).name():
			break;
		case typeid(uint8_t).name():
			break;
		case typeid(uint16_t).name():
			break;
		case typeid(uint32_t).name():
			break;
		case typeid(uint64_t).name():
			break;
		default:
			throw std::exception("Unsupported variable type: %s", typeid(var).name());
		}
		m_Packet->appendData((uint8_t*)&enumeration, 2, PACKET_VARIABLE_NAME);
		m_Packet->appendData(var);
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