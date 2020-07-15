#include "pch.h"
#include "Packet.h"

uint16_t metaDataChunkSize = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t); // metedata semantics ::: VARIABLE INDEX | VARIABLE SIZE | VARIABLE TYPE
uint16_t headerDataSize;
// header semantics ::: PACKET NUMBER | ORDERED? | ENCRYPTED? | AWAIT ACK? | PACKET TYPE | HEADER LENGTH | DATA 0TH INDEX | DATA LENGTH | METADATA 0TH INDEX | METADATA LENGTH

Packet::Packet(bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum)
{
	headerDataSize = sizeof(uint32_t) + sizeof(bool) + sizeof(bool) + sizeof(bool) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

	appendedBytes = 0;

	header[0] = (((uint8_t*)&packetNum)[3]); // PACKET NUMBER
	header[1] = (((uint8_t*)&packetNum)[2]);
	header[2] = (((uint8_t*)&packetNum)[1]);
	header[3] = (((uint8_t*)&packetNum)[0]);

	header[4] = (ordered); // ORDERED?

	header[5] = (encrypted); // ENCRYPTED?

	header[6] = (awaitACK); // AWAIT ACK?

	header[7] = (typeId); // PACKET TYPE

	header[8] = (((uint8_t*)&headerDataSize)[1]);
	header[9] = (((uint8_t*)&headerDataSize)[0]);

	uint32_t hdr32int = headerDataSize; // DATA 0TH INDEX
	header[10] = (((uint8_t*)&hdr32int)[3]);
	header[11] = (((uint8_t*)&hdr32int)[2]);
	header[12] = (((uint8_t*)&hdr32int)[1]);
	header[13] = (((uint8_t*)&hdr32int)[0]);

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

/*
Packet::~Packet()
{
	//delete data;
}
*/

void Packet::setCompleteData(uint8_t* hdr, uint16_t hdrLen, std::vector<uint8_t> payload, uint32_t payloadLen, std::vector<uint8_t> meta, uint16_t metaLen)
{
	memcpy(header, hdr, 26);
	data = payload;
	this->meta = meta;

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
	appendedMetaBytes += metaDataChunkSize;
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
	return appendData((uint8_t*)&idata, sizeof(uint64_t), 6);
}

void Packet::setData(std::vector<uint8_t> bytes)
{
	data = bytes;
}

std::vector<uint8_t> Packet::getMetaData()
{
	return meta;
}

uint32_t Packet::getMetaLength()
{
	return meta.size();
}

uint8_t* Packet::getHeaderData()
{
	return header;
}

uint32_t Packet::getHeaderLength()
{
	return 26;
}

std::vector<uint8_t> Packet::getData()
{
	return data;
}

uint32_t Packet::getDataLength()
{
	return data.size();
}

std::vector<uint8_t> Packet::getFullPacket()
{
	std::vector<uint8_t> completePacket;

	uint32_t dataLen = data.size();
	header[14] = (((uint8_t*)&(dataLen))[3]); // how much data do we have?
	header[15] = (((uint8_t*)&(dataLen))[2]);
	header[16] = (((uint8_t*)&(dataLen))[1]);
	header[17] = (((uint8_t*)&(dataLen))[0]);

	uint32_t metaDataint = data.size() + 26; // INDEX 0TH METADATA
	header[18] = (((uint8_t*)&(metaDataint))[3]);
	header[19] = (((uint8_t*)&(metaDataint))[2]);
	header[20] = (((uint8_t*)&(metaDataint))[1]);
	header[21] = (((uint8_t*)&(metaDataint))[0]);

	uint32_t metasize = meta.size();
	header[22] = (((uint8_t*)&(metasize))[3]); // METADATA LENGTH
	header[23] = (((uint8_t*)&(metasize))[2]);
	header[24] = (((uint8_t*)&(metasize))[1]);
	header[25] = (((uint8_t*)&(metasize))[0]);

	completePacket.reserve(26 + data.size() + meta.size());
	for (int i = 0; i < 26; i++)
	{
		completePacket.push_back(header[i]);
	}
	completePacket.insert(completePacket.end(), data.begin(), data.end());
	completePacket.insert(completePacket.end(), meta.begin(), meta.end());

	return completePacket;
}

uint32_t Packet::getFullPacketLength()
{
	return 26 + data.size() + meta.size();
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
	return (data[location] << 8 | data[location+1]);
}

uint32_t Packet::get32AtLocation(int location)
{
	return ((uint32_t)(data[location + 3] << 24) | (uint32_t)(data[location + 2] << 16) | (uint32_t)(data[location + 1] << 8) | (uint32_t)(data[location]));
}

uint64_t Packet::get64AtLocation(int location)
{
	return (((uint64_t)(data[(long)location + 7])) << 56) | (((uint64_t)(data[(long)location + 6])) << 48) | (((uint64_t)(data[(long)location + 5])) << 40) | (((uint64_t)(data[(long)location + 4])) << 32) |
		(((uint64_t)(data[(long)location + 3])) << 24) | (((uint64_t)(data[(long)location + 2])) << 16) | (((uint64_t)(data[(long)location + 1])) << 8) | ((uint64_t)(data[(long)location]));
}

double Packet::getDoubleAtLocation(int location)
{
	double* p = reinterpret_cast<double*>(get64AtLocation(location));
	return *p;
}

float Packet::getFloatAtLocation(int location)
{
	float* p = reinterpret_cast<float*>(get32AtLocation(location));
	return *p;
}
