#pragma once

#include "pch.h"
#include <string>
#include <array>
#include <iostream>
#include "CommonDefinitions.h"
#include <WinSock2.h>
#include <winerror.h>
#include <ws2tcpip.h>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Utils
{
public:
	static std::string exec(const char* cmd);
	static sockaddr_in MakeAddress(std::string IP, uint16_t Port);
	static std::string ByteAddrToString(uint8_t* addr);

	struct PortTranslation
	{
		PortTranslation()
		{
			OutboundLocal = 0;
			OutboundPublic = 0;
		}
		PortTranslation(uint16_t _local, uint16_t _public)
		{
			OutboundLocal = _local;
			OutboundPublic = _public;
		}

		uint16_t OutboundLocal;
		uint16_t OutboundPublic;
	};

	struct Endpoint
	{
		Endpoint()
		{
			IP = "";
			privateKey = 0;
			portPair = Utils::PortTranslation();
		}
		Endpoint(std::string addr, uint64_t key, Utils::PortTranslation pt)
		{
			IP = addr;
			privateKey = key;
			portPair = pt;
		}
		std::string IP;
		uint64_t privateKey;
		Utils::PortTranslation portPair;
	};

	static PortTranslation getPortTranslation(uint16_t Port, std::string stunServer);
	static void getPublicIPAddress(std::string stunServer, uint8_t* ipAddr);
	static void getIPFromString(std::string ip,  uint8_t* ipAddr);
	static std::string getStringFromIP(uint8_t* ip);

	static std::string stringFromBytes(uint8_t* bytes, uint32_t size);
	static void bytesFromString(std::string string, uint32_t* p_size, uint8_t* bytes);
	static std::vector<uint8_t> vectorBytesFromString(std::string string);
	static std::string stringFromVectorBytes(std::vector<uint8_t> bytes);
};

