#include "pch.h"
#include "Packet.h"

uint16_t metaDataChunkSize = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t); // metedata semantics ::: VARIABLE INDEX | VARIABLE SIZE | VARIABLE TYPE
uint16_t headerDataSize;
// header semantics ::: PACKET NUMBER | ORDERED? | ENCRYPTED? | AWAIT ACK? | PACKET TYPE | HEADER LENGTH | DATA 0TH INDEX | DATA LENGTH | METADATA 0TH INDEX | METADATA LENGTH

Packet::Packet(uint32_t packetSize, bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum)
{
	headerDataSize = sizeof(uint32_t) + sizeof(bool) + sizeof(bool) + sizeof(bool) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

	data = (uint8_t*)malloc(packetSize); // allocate space for the data
	if (data == nullptr) throw std::runtime_error("packet data/payload initialization failed");
	memset(data, 0, packetSize);

	meta = (uint8_t*)malloc(1);
	if (meta == nullptr) throw std::runtime_error("packet meta initialization failed");
	memset(meta, 0, 1);

	header = (uint8_t*)malloc(headerDataSize); // ALLOCATING HEADER
	if (header == nullptr) throw std::runtime_error("packet header initialization failed");
	memset(header, 0, headerDataSize);

	dataBytes = packetSize;
	appendedBytes = 0;

	uint8_t* pktNumuint = Convert32(packetNum); // convert to byte array
	header[0] = pktNumuint[0]; // PACKET NUMBER
	header[1] = pktNumuint[1];
	header[2] = pktNumuint[2];
	header[3] = pktNumuint[3];

	header[4] = ordered; // ORDERED?

	header[5] = encrypted; // ENCRYPTED?

	header[6] = awaitACK; // AWAIT ACK?

	header[7] = typeId; // PACKET TYPE

	uint8_t* headerDataSizeuint = Convert16(headerDataSize); // convert to byte array
	header[8] = headerDataSizeuint[0]; // HEADER LENGTH
	header[9] = headerDataSizeuint[1];

	uint32_t hdr32int = headerDataSize; // DATA 0TH INDEX
	uint8_t* hdr32intuint = Convert32(hdr32int); // convert to byte array
	header[10] = hdr32intuint[0];
	header[11] = hdr32intuint[1];
	header[12] = hdr32intuint[2];
	header[13] = hdr32intuint[3];

}

Packet::~Packet()
{
	delete data;
}

void Packet::setCompleteData(uint8_t* hdr, uint16_t hdrLen, uint8_t* payload, uint32_t payloadLen, uint8_t* meta, uint16_t metaLen)
{
	memcpy(header, hdr, hdrLen);
	memcpy(data, payload, payloadLen);
	memcpy(this->meta, meta, metaLen);

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

	if (dataBytes <= appendIndex) //if the data array is too small, allocate more space
	{
		uint8_t* newData = new uint8_t[appendedBytes];
		memcpy(newData, data, appendIndex);
		delete data;
		data = newData;
	}

	appendedMetaBytes += metaDataChunkSize; // resize metadata no matter what
	uint8_t* newMeta = new uint8_t[appendedMetaBytes];
	memcpy(newMeta, meta, appendedMetaBytes - appendedMetaBytes);
	delete meta;
	meta = newMeta;

	uint8_t* appendIndexuint = Convert32(appendIndex);
	meta[appendedMetaBytes - metaDataChunkSize + 0] = appendIndexuint[0]; // make metadata and put it in an array semantically
	meta[appendedMetaBytes - metaDataChunkSize + 1] = appendIndexuint[1];
	meta[appendedMetaBytes - metaDataChunkSize + 2] = appendIndexuint[2];
	meta[appendedMetaBytes - metaDataChunkSize + 3] = appendIndexuint[3];
	
	uint8_t* dataSizeuint = Convert16(dataSize);
	meta[appendedMetaBytes - metaDataChunkSize + 4] = dataSizeuint[0];
	meta[appendedMetaBytes - metaDataChunkSize + 5] = dataSizeuint[1];

	meta[appendedMetaBytes - metaDataChunkSize + 6] = type;

	for (int i = 0; i < size; i++) // place the input data into the array at the next possible location
	{
		data[appendedBytes - size + i] = idata[i];
	}

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

uint8_t* Packet::getMetaData()
{
	return meta;
}

uint32_t Packet::getMetaLength()
{
	return appendedMetaBytes;
}

uint8_t* Packet::getHeaderData()
{
	return header;
}

uint32_t Packet::getHeaderLength()
{
	return headerDataSize;
}

uint8_t* Packet::getData()
{
	return data;
}

uint32_t Packet::getDataLength()
{
	return appendedBytes;
}

uint8_t* Packet::getFullPacket()
{
	trimPacket();
	uint8_t* completePacket = new uint8_t[(uint32_t)headerDataSize + appendedBytes + appendedMetaBytes];

	header[14] = ((uint8_t*)&(appendedBytes))[0]; // DATA LENGTH
	header[15] = ((uint8_t*)&(appendedBytes))[1];
	header[16] = ((uint8_t*)&(appendedBytes))[2];
	header[17] = ((uint8_t*)&(appendedBytes))[3];

	uint32_t metaDataint = appendedBytes + headerDataSize; // INDEX 0TH METADATA
	header[18] = ((uint8_t*)&(metaDataint))[0];
	header[19] = ((uint8_t*)&(metaDataint))[1];
	header[20] = ((uint8_t*)&(metaDataint))[2];
	header[21] = ((uint8_t*)&(metaDataint))[3];

	header[22] = ((uint8_t*)&(appendedMetaBytes))[0]; // METADATA LENGTH
	header[23] = ((uint8_t*)&(appendedMetaBytes))[1];
	header[24] = ((uint8_t*)&(appendedMetaBytes))[2];
	header[25] = ((uint8_t*)&(appendedMetaBytes))[3];

	for (unsigned int i = 0; i < headerDataSize; i++) // laying all the header bytes
	{
		completePacket[i] = header[i];
	}
	for (unsigned int i = 0; i < appendedBytes; i++) // laying all the data bytes
	{
		completePacket[headerDataSize + i] = data[i];
	}
	for (unsigned int i = 0; i < appendedMetaBytes; i++) // laying all the metadata bytes
	{
		completePacket[headerDataSize + appendedMetaBytes + i] = meta[i];
	}

	return completePacket;
}

uint32_t Packet::getFullPacketLength()
{
	return headerDataSize + appendedBytes + appendedMetaBytes;
}

uint32_t Packet::trimPacket()
{
	uint8_t* finalData = new uint8_t[appendedBytes]; // trim and truncate the data
	memcpy(finalData, data, appendedBytes);
	delete data;
	data = finalData;
	return appendedBytes;
}