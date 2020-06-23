// TestApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <EZMP.h>
#include <string>

int main(int argc, char* argv[])
{
	char password[] = "im stuff";
	uint8_t addr[] = {127, 0, 0, 1};

	for (int i = 0; i < argc; ++i)
		std::cout << argv[i] << "\n";

	if (strcmp(argv[1], "server") == 0)
	{
		MPInterfacer* interfacer = new MPInterfacer(123, std::string(password), (uint16_t)49950, addr, (uint16_t)49951);
		printf("server mode");

		while (true)
		{
			Packet received = interfacer->recvPacket();
			printf("Received Packet: %s", received.getData());
		}
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
			send.getFullPacket();
			interfacer->sendPacket(&send);
		}
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
