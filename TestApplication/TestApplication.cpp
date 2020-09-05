// TestApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <EZMP.h>
#include <string>

MPInterfacer* interfacer;

void onPacketReceive(Packet p)
{
	if (p.getPacketType() == NETWORK_TABLE_PACKET)
	{
		NTPacket ntp(&p);
		if (ntp.getPacketEnumeration() == 11112)
		{
			std::string request = ntp.getStringByEnumeration(0);

		}
	}
	/*
	std::string message;
	for (unsigned int i = 0; i < p.getDataLength(); i++) message += p.getData()[i];
	printf("\n\nReceived %d bytes: %s\n", p.getDataLength(), message.c_str());
	interfacer->multicastPacket(&p, interfacer->ServersideEndpoints);
	*/
}

int main(int argc, char* argv[])
{
	char password[] = "im stuff";
	//uint8_t addr[] = { 192,168,11,19 };
	uint8_t addr[] = { 73,2,83,28 };
	//uint8_t addr[] = { 67,164,120,12 };
	//uint8_t addr[] = { 192,168,11,8 };

	EZMP::Init();

	for (int i = 0; i < argc; ++i)
		std::cout << argv[i] << "\n";

	if (strcmp(argv[1], "server") == 0)
	{
		std::string pass;
		std::getline(std::cin, pass);

		interfacer = new MPInterfacer(123, (uint16_t)40007, addr, true, pass);
		interfacer->attachReceiveCallback(&onPacketReceive);
		printf("server mode");
		std::getchar();
	}
	if (strcmp(argv[1], "client") == 0)
	{
		std::string pass;
		std::getline(std::cin, pass);

		interfacer = new MPInterfacer(123, (uint16_t)10000, addr, false, pass);
		printf("client mode\n");
		interfacer->attachReceiveCallback(&onPacketReceive);

		Packet* send;
		NTPacket* ntp;
		while (true)
		{
			ntp = new NTPacket(11111, false, false, true, NETWORK_TABLE_PACKET, 0);

			ntp->appendVariable<std::string>("ADD", 0);
			ntp->appendVariable<std::string>("192.168.11.8", 1);
			ntp->appendVariable<uint16_t>(20002, 2);
			ntp->appendVariable<std::string>(std::to_string(rand() % 30), 3);
			ntp->appendVariable<std::string>("Very Epic gaming server", 4);
			ntp->appendVariable<std::string>("", 5);
			ntp->appendVariable<uint16_t>(3, 6);
			ntp->appendVariable<uint16_t>(10, 7);
			ntp->appendVariable<std::string>("City", 8);

			send = new Packet(false, false, false, 11, 1);
			std::string toSend;
			std::getline(std::cin, toSend);
			//int sendFloat = std::stoi(toSend);
			interfacer->sendPacket(ntp->getBasePacket());
			ntp = new NTPacket(11111, false, false, true, NETWORK_TABLE_PACKET, 0);
			ntp->appendVariable("GET", 0);
			interfacer->sendPacket(ntp->getBasePacket());
		}
		interfacer->~MPInterfacer();
	}
}