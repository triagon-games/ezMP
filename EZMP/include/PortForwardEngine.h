#pragma once
#include <natupnp.h>
#include <comutil.h>

#pragma comment (lib, "comsuppw.lib")

class PortForwardEngine
{
public:
	static void UPnPportForward(long internalPort, long externalPort);
	static void UDPHolePunch(SOCKET sendSock, sockaddr_in sendSockAddr);
};