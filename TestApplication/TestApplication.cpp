// TestApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <EZMP.h>
#include <string>

void onPacketReceive(Packet p)
{
	std::string message;
	for (int i = 0; i < p.getDataLength(); i++) message += p.getData()[i];
	printf("\n\nReceived %d bytes: %s", p.getDataLength(), message.c_str());
}

int main(int argc, char* argv[])
{
	char password[] = "im stuff";
	uint8_t addr[] = {192, 168, 11, 19};

	for (int i = 0; i < argc; ++i)
		std::cout << argv[i] << "\n";

	if (strcmp(argv[1], "server") == 0)
	{
		MPInterfacer* interfacer = new MPInterfacer(123, std::string(password), (uint16_t)49950, addr, (uint16_t)49951);
		interfacer->attachReceiveCallback(&onPacketReceive);
		printf("server mode");
		std::getchar();
	}
	if (strcmp(argv[1], "client") == 0)
	{
		MPInterfacer* interfacer = new MPInterfacer(123, std::string(password), (uint16_t)49951, addr, (uint16_t)49950);
		printf("client mode");
		interfacer->attachReceiveCallback(&onPacketReceive);

		while (true)
		{
			Packet send = Packet(false, false, false, 1, 1);
			std::string toSend;
			std::getline(std::cin, toSend);
			send.appendData(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(toSend.c_str())), toSend.length());
			interfacer->sendPacket(&send);
		}
	}
}