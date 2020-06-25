#include "pch.h"
#include "Packet.h"

uint16_t metaDataChunkSize = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t); // metedata semantics ::: VARIABLE INDEX | VARIABLE SIZE | VARIABLE TYPE
uint16_t headerDataSize;
// header semantics ::: PACKET NUMBER | ORDERED? | ENCRYPTED? | AWAIT ACK? | PACKET TYPE | HEADER LENGTH | DATA 0TH INDEX | DATA LENGTH | METADATA 0TH INDEX | METADATA LENGTH

Packet::Packet(bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum)
{
	headerDataSize = sizeof(uint32_t) + sizeof(bool) + sizeof(bool) + sizeof(bool) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

	appendedBytes = 0;

	header.push_back(((uint8_t*)&packetNum)[3]); // PACKET NUMBER
	header.push_back(((uint8_t*)&packetNum)[2]);
	header.push_back(((uint8_t*)&packetNum)[1]);
	header.push_back(((uint8_t*)&packetNum)[0]);

	header.push_back(ordered); // ORDERED?

	header.push_back(encrypted); // ENCRYPTED?

	header.push_back(awaitACK); // AWAIT ACK?

	header.push_back(typeId); // PACKET TYPE

	header.push_back(((uint8_t*)&headerDataSize)[1]);
	header.push_back(((uint8_t*)&headerDataSize)[0]);

	uint32_t hdr32int = headerDataSize; // DATA 0TH INDEX
	header.push_back(((uint8_t*)&hdr32int)[3]);
	header.push_back(((uint8_t*)&hdr32int)[2]);
	header.push_back(((uint8_t*)&hdr32int)[1]);
	header.push_back(((uint8_t*)&hdr32int)[0]);

	packetType = typeId;
	this->ordered = ordered;
	this->encrypted = encrypted;
	this->awaitACK = awaitACK;
	this->packetNum = packetNum;
}

Packet::Packet()
{
	awaitACK = false;
	delivered = false;
	encrypted = false;
	ordered = false;
	packetNum = 0;
	packetType = UNSPECIFIED_PACKET;
}

Packet::~Packet()
{
	//delete data;
}

void Packet::setCompleteData(uint8_t* hdr, uint16_t hdrLen, uint8_t* payload, uint32_t payloadLen, uint8_t* meta, uint16_t metaLen)
{
	for (int i = 0; i < headerDataSize; i++)
	{
		header.push_back(hdr[i]);
	}
	for (int i = 0; i < payloadLen; i++)
	{
		data.push_back(payload[i]);
	}
	for (int i = 0; i < metaLen; i++)
	{
		this->meta.push_back(meta[i]);
	}

	appendedBytes = payloadLen;
	appendedMetaBytes = metaLen;
}

/*
 * Appends Data to the network Packet
 *
 * @param whatever data you want to append to the packet
 * @return the index of the variable in the array ... basically a pointer you could use for interpreting received data
 */
uint32_t Packet::appendData(uint8_t idata[], size_t size, uint8_t type)
{
	uint32_t appendIndex = appendedBytes;
	uint16_t dataSize = size;
	appendedBytes += size;

	for (int i = 0; i < size; i++)
	{
		data.push_back(idata[i]);
	}

	meta.push_back(((uint8_t*)&appendIndex)[3]);
	meta.push_back(((uint8_t*)&appendIndex)[2]);
	meta.push_back(((uint8_t*)&appendIndex)[1]);
	meta.push_back(((uint8_t*)&appendIndex)[0]);
	
	meta.push_back(((uint8_t*)&size)[1]);
	meta.push_back(((uint8_t*)&size)[0]);

	meta.push_back(type);

	return appendedBytes-size; // returns the index of the first byte of the variable in the data array
}

uint32_t Packet::appendData(float idata)
{
	return appendData(ConvertFloat(idata), sizeof(float), 1);
}

uint32_t Packet::appendData(double idata)
{
	return appendData(ConvertDouble(idata), 2);
}

uint32_t Packet::appendData(uint8_t idata)
{
	return appendData(&(idata), sizeof(uint8_t), 3);
}

uint32_t Packet::appendData(uint16_t idata)
{
	return appendData(Convert16(idata), sizeof(uint16_t), 4);
}

uint32_t Packet::appendData(uint32_t idata)
{
	return appendData(Convert32(idata), sizeof(uint32_t), 5);
}

uint32_t Packet::appendData(uint64_t idata)
{
	return appendData(Convert64(idata), sizeof(uint64_t), 6);
}

std::vector<uint8_t> Packet::getMetaData()
{
	return meta;
}

uint32_t Packet::getMetaLength()
{
	return appendedMetaBytes;
}

std::vector<uint8_t> Packet::getHeaderData()
{
	return header;
}

uint32_t Packet::getHeaderLength()
{
	return headerDataSize;
}

std::vector<uint8_t> Packet::getData()
{
	return data;
}

uint32_t Packet::getDataLength()
{
	return appendedBytes;
}

std::vector<uint8_t> Packet::getFullPacket()
{
	std::vector<uint8_t> completePacket;

	uint32_t dataLen = data.size();
	header.push_back(((uint8_t*)&(dataLen))[3]); // how much data do we have?
	header.push_back(((uint8_t*)&(dataLen))[2]);
	header.push_back(((uint8_t*)&(dataLen))[1]);
	header.push_back(((uint8_t*)&(dataLen))[0]);

	uint32_t metaDataint = appendedBytes + headerDataSize; // INDEX 0TH METADATA
	header.push_back(((uint8_t*)&(metaDataint))[3]);
	header.push_back(((uint8_t*)&(metaDataint))[2]);
	header.push_back(((uint8_t*)&(metaDataint))[1]);
	header.push_back(((uint8_t*)&(metaDataint))[0]);

	header.push_back(((uint8_t*)&(appendedMetaBytes))[3]); // METADATA LENGTH
	header.push_back(((uint8_t*)&(appendedMetaBytes))[2]);
	header.push_back(((uint8_t*)&(appendedMetaBytes))[1]);
	header.push_back(((uint8_t*)&(appendedMetaBytes))[0]);

	completePacket.reserve(header.size() + data.size() + meta.size());
	completePacket.insert(completePacket.end(), header.begin(), header.end());
	completePacket.insert(completePacket.end(), data.begin(), data.end());
	completePacket.insert(completePacket.end(), meta.begin(), meta.end());

	return completePacket;
}

uint32_t Packet::getFullPacketLength()
{
	return headerDataSize + appendedBytes + appendedMetaBytes;
}

uint8_t Packet::getPacketType()
{
	return packetType;
}

uint32_t Packet::getPacketNum()
{
	return packetNum;
}

bool Packet::isOrdered()
{
	return ordered;
}

bool Packet::isEncrypted()
{
	return encrypted;
}

bool Packet::isAwaitACK()
{
	return awaitACK;
}

bool Packet::isDelivered()
{
	return delivered;
}

void Packet::Deliver()
{
	delivered = true;
}

uint8_t Packet::get8AtLocation(int location)
{
	return data[location];
}

uint16_t Packet::get16AtLocation(int location)
{
	return (data[location] << 8 | data[(long)location+1]);
}

uint32_t Packet::get32AtLocation(int location)
{
	return ((uint32_t)(data[location] << 24) | (uint32_t)(data[(long)location] << 16) | (uint32_t)(data[(long)location] << 8) | (uint32_t)(data[(long)location]));
}

uint64_t Packet::get64AtLocation(int location)
{
	return uint64_t();
}

double Packet::getDoubleAtLocation(int location)
{
	return 0.0;
}

float Packet::getFloatAtLocation(int location)
{
	return 0.0f;
}
