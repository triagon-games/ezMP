#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <natupnp.h>
#include <comutil.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#include "pch.h"
#include "Utils.h"

#include <vector>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment (lib, "comsuppw.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class PortForwardEngine
{
public:
	static bool UPnPportForward(long internalPort, long externalPort);
	static void UDPHolePunch(SOCKET sendSock, sockaddr_in sendSockAddr);
};