#include "NTPacket.h"

NTPacket::NTPacket(Packet* p) // deriving NTPacket from Packet
{
	m_Packet = p;
	m_packetID = m_Packet->get16AtLocation(0);
}

NTPacket::NTPacket(uint16_t id, Packet* p)
{
	m_Packet = p;
	m_packetID = id;
}

NTPacket::NTPacket(uint16_t id, bool multicastable, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum)
{
	m_packetID = id;
	m_Packet = new Packet(multicastable, encrypted, awaitACK, typeId, packetNum);
	m_Packet->appendData((uint8_t*)&m_packetID, 2, PACKET_ENUMERATOR);
}

uint16_t NTPacket::getPacketEnumeration()
{
	return m_Packet->get16AtLocation(0);
}

Packet* NTPacket::getBasePacket()
{
	return m_Packet;
}

template<typename T>
T NTPacket::getVariableByEnumeration(uint16_t id)
{
	std::vector<VariableData> enumerators;
	std::vector<VariableData> variables;
	std::vector<VariableData> alldata = m_Packet->getVariables();
	for (int i = 0; i < alldata.size(); i++)
	{
		if (alldata[i].variableType == PACKET_VARIABLE_NAME)
		{
			enumerators.push_back(alldata[i]);
			variables.push_back(alldata[i + 1]);
		}
	}
	for (int i = 0; i < enumerators.size(); i++)
	{
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id)
		{
			switch (variables[i].variableType)
			{
			case PACKET_FLOAT:
				return m_Packet->getFloatAtLocation(variables[i].startIndex);
				break;
			case PACKET_DOUBLE:
				return m_Packet->getDoubleAtLocation(variables[i].startIndex);
				break;
			case PACKET_BYTE:
				return m_Packet->get8AtLocation(variables[i].startIndex);
				break;
			case PACKET_U16:
				return m_Packet->get16AtLocation(variables[i].startIndex);
				break;
			case PACKET_U32:
				return m_Packet->get32AtLocation(variables[i].startIndex);
				break;
			case PACKET_U64:
				return m_Packet->get64AtLocation(variables[i].startIndex);
				break;
			}
		}
	}
	return -1.0001f;
}

template<typename T>
uint32_t NTPacket::appendVariable(T var, uint16_t enumeration)
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
