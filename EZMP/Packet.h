#pragma once
#include "pch.h"
#include <cstddef>
class Packet
{
public:
    Packet(uint32_t packetSize);
    ~Packet();

    uint32_t appendData(uint8_t idata[]);
    uint32_t appendData(float diata);
    uint32_t appendData(double idata);
    uint32_t appendData(uint8_t idata);
    uint32_t appendData(uint16_t idata);
    uint32_t appendData(uint32_t idata);
    uint32_t appendData(uint64_t idata);

    uint8_t* getFullPacket();
    void trimPacket();

private:
    uint8_t* data;
    uint32_t dataBytes;
    uint32_t appendedBytes;
};