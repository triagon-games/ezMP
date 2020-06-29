#pragma once
#include <natupnp.h>
#include <comutil.h>

#pragma comment (lib, "comsuppw.lib")

class PortForwardEngine
{
public:
	static void portForward(long internalPort, long externalPort);
};