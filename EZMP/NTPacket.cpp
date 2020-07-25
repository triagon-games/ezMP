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

EZMP_DLL float NTPacket::getFloatByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_FLOAT)
		{
			return m_Packet->getFloatAtLocation(variables[i].startIndex);
		}
	}
	return NULL;
}

EZMP_DLL double NTPacket::getDoubleByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_DOUBLE)
		{
			return m_Packet->getDoubleAtLocation(variables[i].startIndex);
		}
	}
	return NULL;
}

EZMP_DLL uint8_t NTPacket::getByteByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_BYTE)
		{
			return m_Packet->get8AtLocation(variables[i].startIndex);
		}
	}
	return NULL;
}

EZMP_DLL uint16_t NTPacket::getShortByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_DOUBLE)
		{
			return m_Packet->getDoubleAtLocation(variables[i].startIndex);
		}
	}
	return NULL;
}

EZMP_DLL uint32_t NTPacket::getIntByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_U32)
		{
			return m_Packet->get32AtLocation(variables[i].startIndex);
		}
	}
	return NULL;
}

EZMP_DLL uint64_t NTPacket::getLongByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_U64)
		{
			return m_Packet->getDoubleAtLocation(variables[i].startIndex);
		}
	}
	return NULL;
}

EZMP_DLL std::string NTPacket::getStringByEnumeration(uint16_t id)
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
		if (m_Packet->get16AtLocation(enumerators[i].startIndex) == id && variables[i].variableType == PACKET_DOUBLE)
		{
			std::string ret;
			for (int x = variables[i].startIndex; x < variables[i].startIndex + variables[i].variableSize; x++)
			{
				ret.push_back(m_Packet->getData()[x]);
			}
			return ret;
		}
	}
	return "";
}
