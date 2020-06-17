#pragma once
#include "pch.h"
#include <cstddef>
class Packet
{
public:
    Packet(uint32_t packetSize, bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum);
    ~Packet();

    uint32_t appendData(uint8_t idata[], uint8_t type = 0);
    uint32_t appendData(float diata);
    uint32_t appendData(double idata);
    uint32_t appendData(uint8_t idata);
    uint32_t appendData(uint16_t idata);
    uint32_t appendData(uint32_t idata);
    uint32_t appendData(uint64_t idata);

    uint8_t* getMetaData();
    uint8_t* getHeaderData();
    uint8_t* getData();
    uint8_t* getFullPacket();
    uint32_t trimPacket();

private:
    uint8_t* header;
    uint8_t* data;
    uint8_t* meta;
    uint32_t dataBytes = 0;
    uint32_t appendedBytes = 0;
    uint32_t appendedMetaBytes = 0;
};