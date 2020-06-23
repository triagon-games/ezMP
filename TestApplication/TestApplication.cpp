// TestApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <EZMP.h>
#include <string>

void onPacketReceive(Packet p)
{
	printf("Received %d bytes: %s", p.getDataLength(), (const char*)p.getData());
}

int main(int argc, char* argv[])
{
	char password[] = "im stuff";
	uint8_t addr[] = {127, 0, 0, 1};

	for (int i = 0; i < argc; ++i)
		std::cout << argv[i] << "\n";

	if (strcmp(argv[1], "server") == 0)
	{
		MPInterfacer* interfacer = new MPInterfacer(123, std::string(password), (uint16_t)49950, addr, (uint16_t)49951);
		interfacer->attachReceiveCallback(&onPacketReceive);
		printf("server mode");
	}
	if (strcmp(argv[1], "client") == 0)
	{
		MPInterfacer* interfacer = new MPInterfacer(123, std::string(password), (uint16_t)49951, addr, (uint16_t)4995);
		printf("client mode");

		while (true)
		{
			Packet send = Packet(1000, false, false, false, 1, 1);
			std::string toSend;
			std::getline(std::cin, toSend);
			send.appendData(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(toSend.c_str())), toSend.length());
			interfacer->sendPacket(&send);
		}
	}
}