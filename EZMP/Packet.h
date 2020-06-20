#pragma once
#include "pch.h"
#include <cstddef>
#include "DataStructures.h"

class Packet
{
public:
    Packet(uint32_t packetSize, bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum);
    ~Packet();

    void setCompleteData(uint8_t* hdr, uint16_t hdrLen, uint8_t* payload, uint32_t payloadLen, uint8_t* meta, uint16_t metaLen);
    uint32_t appendData(uint8_t idata[], size_t size, uint8_t type = 0);
    uint32_t appendData(float diata);
    uint32_t appendData(double idata);
    uint32_t appendData(uint8_t idata);
    uint32_t appendData(uint16_t idata);
    uint32_t appendData(uint32_t idata);
    uint32_t appendData(uint64_t idata);

    uint8_t* getMetaData();
    uint32_t getMetaLength();
    uint8_t* getHeaderData();
    uint32_t getHeaderLength();
    uint8_t* getData();
    uint32_t getDataLength();
    uint8_t* getFullPacket();
    uint32_t getFullPacketLength();
    uint32_t trimPacket();

    uint8_t* sourceAddr = new uint8_t[4];
    uint16_t sourcePort = NULL;

private:
    uint8_t* header = nullptr;
    uint8_t* data = nullptr;
    uint8_t* meta = nullptr;
    uint32_t dataBytes = 0;
    uint32_t appendedBytes = 0;
    uint32_t appendedMetaBytes = 0;
};