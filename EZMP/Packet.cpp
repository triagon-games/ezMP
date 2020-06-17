#include "pch.h"
#include "Packet.h"

uint16_t metaDataChunkSize = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t); // metedata semantics ::: VARIABLE INDEX | VARIABLE SIZE | VARIABLE TYPE
uint16_t headerDataSize = sizeof(uint32_t) + sizeof(bool) + sizeof(bool) + sizeof(bool) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
// header semantics ::: PACKET NUMBER | ORDERED? | ENCRYPTED? | AWAIT ACK? | PACKET TYPE | HEADER LENGTH | DATA 0TH INDEX | DATA LENGTH | METADATA 0TH INDEX | METADATA LENGTH

Packet::Packet(uint32_t packetSize, bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum)
{
	data = (uint8_t*)malloc(packetSize); // allocate space for the data
	dataBytes = 0;
	appendedBytes = 0;

	header = (uint8_t*)malloc(headerDataSize);

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
uint32_t Packet::appendData(uint8_t idata[], uint8_t type = 0)
{
	appendedBytes += sizeof(idata);

	if (dataBytes <= appendedBytes - sizeof(idata)) //if the data array is too small, allocate more space
	{
		uint8_t* newData = new uint8_t[appendedBytes];
		memcpy(newData, data, appendedBytes - sizeof(idata));
		delete data;
		data = newData;
	}

	appendedMetaBytes += metaDataChunkSize;
	uint8_t* newMeta = new uint8_t[appendedMetaBytes];
	memcpy(newMeta, meta, appendedMetaBytes - appendedMetaBytes);
	delete meta;
	meta = newMeta;

	for (int i = 0; i < sizeof(idata); i++) // place the input data into the array at the next possible location
	{
		data[appendedBytes - sizeof(idata) + i] = idata[i];
	}

	return appendedBytes-sizeof(idata); // returns the index of the first byte of the variable in the data array
}

uint32_t Packet::appendData(float idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata), 1);
}

uint32_t Packet::appendData(double idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata), 2);
}

uint32_t Packet::appendData(uint8_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata), 3);
}

uint32_t Packet::appendData(uint16_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata), 4);
}

uint32_t Packet::appendData(uint32_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata), 5);
}

uint32_t Packet::appendData(uint64_t idata)
{
	return appendData(reinterpret_cast<uint8_t*>(&idata), 6);
}

uint8_t* Packet::getMetaData()
{
	return nullptr;
}

uint8_t* Packet::getHeaderData()
{
	return nullptr;
}

uint8_t* Packet::getData()
{
	return data;
}

uint8_t* Packet::getFullPacket()
{
	return nullptr;
}

uint32_t Packet::trimPacket()
{

}