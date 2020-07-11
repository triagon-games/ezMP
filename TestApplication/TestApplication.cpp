// TestApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <EZMP/EZMP.h>
#include <string>

void onPacketReceive(Packet p)
{
	std::string message;
	for (unsigned int i = 0; i < p.getDataLength(); i++) message += p.getData()[i];
	printf("\n\nReceived %d bytes: %s\n", p.getDataLength(), message.c_str());
}

int main(int argc, char* argv[])
{
	char password[] = "im stuff";
	//uint8_t addr[] = { 192,168,11,19 };
	uint8_t addr[] = { 73,162,31,175 };
	//uint8_t addr[] = { 67,164,120,12 };

	EZMP::Init();

	for (int i = 0; i < argc; ++i)
		std::cout << argv[i] << "\n";

	if (strcmp(argv[1], "server") == 0)
	{
		std::string pass;
		std::getline(std::cin, pass);

		MPInterfacer* interfacer = new MPInterfacer(123, (uint16_t)40007, addr, true, pass);
		interfacer->attachReceiveCallback(&onPacketReceive);
		printf("server mode");
		std::getchar();
	}
	if (strcmp(argv[1], "client") == 0)
	{
		std::string pass;
		std::getline(std::cin, pass);

		MPInterfacer* interfacer = new MPInterfacer(123, (uint16_t)40007, addr, false, pass);
		printf("client mode\n");
		interfacer->attachReceiveCallback(&onPacketReceive);

		Packet* send;
		while (true)
		{
			send = new Packet(false, false, true, 4, 1);
			std::string toSend;
			std::getline(std::cin, toSend);
			send->appendData((uint8_t*)toSend.c_str(), toSend.size());
			interfacer->sendPacket(send);
		}
	}
}