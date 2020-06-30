#pragma once
#include "pch.h"
#include <cstddef>
#include "DataStructures.h"
#include <vector>

class Packet
{
public:
    Packet(bool ordered, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum);
    Packet();
    ~Packet();

    void setCompleteData(std::vector<uint8_t> hdr, uint16_t hdrLen, std::vector<uint8_t> payload, uint32_t payloadLen, std::vector<uint8_t> meta, uint16_t metaLen);
    uint32_t appendData(uint8_t idata[], size_t size, uint8_t type = 0);
    uint32_t appendData(float diata);
    uint32_t appendData(double idata);
    uint32_t appendData(uint8_t idata);
    uint32_t appendData(uint16_t idata);
    uint32_t appendData(uint32_t idata);
    uint32_t appendData(uint64_t idata);

    std::vector<uint8_t> getMetaData();
    uint32_t getMetaLength();
    std::vector<uint8_t> getHeaderData();
    uint32_t getHeaderLength();
    std::vector<uint8_t> getData();
    uint32_t getDataLength();
    std::vector<uint8_t> getFullPacket();
    uint32_t getFullPacketLength();

    uint8_t getPacketType();
    uint32_t getPacketNum();
    bool isOrdered();
    bool isEncrypted();
    bool isAwaitACK();
    bool isDelivered();
    void Deliver();

    uint64_t timeSent = 0;

    uint8_t* sourceAddr = new uint8_t[4];
    uint16_t sourcePort = NULL;

    uint8_t get8AtLocation(int location);
    uint16_t get16AtLocation(int location);
    uint32_t get32AtLocation(int location);
    uint64_t get64AtLocation(int location);
    double getDoubleAtLocation(int location);
    float getFloatAtLocation(int location);

private:
    std::vector<uint8_t> header;
    std::vector<uint8_t> data;
    std::vector<uint8_t> meta;
    uint32_t dataBytes = 0;
    uint32_t appendedBytes = 0;
    uint32_t appendedMetaBytes = 0;

    uint32_t packetNum = 0;
    uint8_t packetType = 0;
    bool ordered = false;
    bool encrypted = false;
    bool awaitACK = false;
    bool delivered = false;
};