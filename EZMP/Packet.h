#pragma once
#include "pch.h"
#include <cstddef>
#include "DataStructures.h"
#include <vector>
#include "Utils.h"

struct VariableData
{
public:
    uint32_t startIndex;
    uint16_t variableSize;
    uint8_t variableType;
};

class Packet
{
public:

    EZMP_DLL Packet(bool multicastable, bool encrypted, bool awaitACK, uint8_t typeId, uint32_t packetNum);
    Packet();
    /*
    ~Packet();
    */
    EZMP_DLL void setCompleteData(uint8_t* hdr, uint16_t hdrLen, std::vector<uint8_t> payload, uint32_t payloadLen, std::vector<uint8_t> meta, uint16_t metaLen);
    EZMP_DLL uint32_t appendData(uint8_t idata[], size_t size, uint8_t type = 0);
    EZMP_DLL uint32_t appendData(float diata);
    EZMP_DLL uint32_t appendData(double idata);
    EZMP_DLL uint32_t appendData(uint8_t idata);
    EZMP_DLL uint32_t appendData(uint16_t idata);
    EZMP_DLL uint32_t appendData(uint32_t idata);
    EZMP_DLL uint32_t appendData(uint64_t idata);

    EZMP_DLL void setData(std::vector<uint8_t> bytes);

    EZMP_DLL std::vector<uint8_t> getMetaData();
    EZMP_DLL uint32_t getMetaLength();
    EZMP_DLL uint8_t* getHeaderData();
    EZMP_DLL uint32_t getHeaderLength();
    EZMP_DLL std::vector<uint8_t> getData();
    EZMP_DLL uint32_t getDataLength();
    EZMP_DLL std::vector<uint8_t> getFullPacket();
    EZMP_DLL uint32_t getFullPacketLength();
    EZMP_DLL std::vector<VariableData> getVariables();

    EZMP_DLL uint8_t getPacketType();
    EZMP_DLL uint32_t getPacketNum();
    EZMP_DLL bool isOrdered();
    EZMP_DLL bool isEncrypted();
    EZMP_DLL bool isAwaitACK();
    EZMP_DLL bool isDelivered();
    EZMP_DLL void Deliver();

    uint64_t timeSent = 0;

    uint8_t* sourceAddr = new uint8_t[4];
    uint16_t sourcePort = NULL;

    EZMP_DLL uint8_t get8AtLocation(int location);
    EZMP_DLL uint16_t get16AtLocation(int location);
    EZMP_DLL uint32_t get32AtLocation(int location);
    EZMP_DLL uint64_t get64AtLocation(int location);
    EZMP_DLL double getDoubleAtLocation(int location);
    EZMP_DLL float getFloatAtLocation(int location);

    Utils::Endpoint source;

private:
    uint8_t header[26];
    std::vector<uint8_t> data;
    std::vector<uint8_t> meta;
    std::vector<VariableData> variables;
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