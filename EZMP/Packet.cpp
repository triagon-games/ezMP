#include "pch.h"
#include "Packet.h"

Packet::Packet(uint32_t packetSize)
{
	data = (uint8_t*)malloc(packetSize); // allocate space for the data

}

Packet::~Packet()
{
	delete data;
}

/*
 * Appends Data to the network Packet
 *
 * @param whatever data you want to append to the packet
 * @return the index of the variable in the array
 */
uint32_t Packet::appendData(uint8_t idata[])
{
	appendedBytes += sizeof(idata);

	if (sizeof(data) <= appendedBytes) //if the data array is too small, allocate more space
	{
		uint8_t* newData = new uint8_t[appendedBytes];
		memcpy(newData, data, appendedBytes);
		delete data;
		data = newData;
	}

	for (int i = 0; i < sizeof(idata); i++) // place the input data into the array at the next possible location
	{
		data[appendedBytes - sizeof(idata) + i] = idata[i];
	}

	return appendedBytes-sizeof(idata); // returns the index of the first byte of the variable in the data array
}

uint32_t Packet::appendData(float idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata));
}

uint32_t Packet::appendData(double idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata));
}

uint32_t Packet::appendData(uint8_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata));
}

uint32_t Packet::appendData(uint16_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata));
}

uint32_t Packet::appendData(uint32_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata));
}

uint32_t Packet::appendData(uint64_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata));
}

uint8_t* Packet::getFullPacket()
{
	return nullptr;
}

void Packet::trimPacket()
{
}