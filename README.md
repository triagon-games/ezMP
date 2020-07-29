# ezMP
Easy WIN10 Multiplayer.

For a basic use example, please see `TestApplication.cpp`.

This library supports hole punching, and direct reply packets.
When a packet is received on either end, and IP address and and External-Outbound port can be accessed in the `Packet` class.

Although, it is unrealiable, it is possible to portforward using the upnp functionality. This is done automatically on startup.


## Enabling UPnP port mapping

* Download source code
* Unzip it
* Launch the `.sln` file using VS2019
* Goto `PortForwardEngine.cpp`
* Replace the ip address in the line `BSTR clientName = _com_util::ConvertStringToBSTR("192.168.3.1");` with your machine's local ip address
* Call the function wherever you want lol
* Select the proper configuration type
* Before building, ensure you have `wget` installed as a console command
* Build


## Linking EZMP to your project

* Download the release
* Unzip it
* Select Your Configuration
* Link to includes and include them in code
* Link the library
* If you are not linking a `DLL` file, go to preprocessor macros and define `_LIB`
* In the folder where your executable is, make sure you have a `Stun` folder with all of the `stun` binaries
  * If you dont have it goto the `bin` folder in the archive and copy the `Stun` folder from there
  * Stun is not necessary, but to disable it's use you have to rebuild the library source code and remove the line `28`and `29` in `MPInterface.cpp`
  

## Example program:

*NOTE: THIS PROGRAM DOES NOT USE ALL OF THE FUNCTIONALITY*

**2 in 1 Code for both client and server, just specify either `server` or `client` in the command line**

```cpp
#include <iostream>
#include <EZMP.h>
#include <string>

MPInterfacer* interfacer;

void onPacketReceive(Packet p)
{
	std::string message;
  // there is a simpler way to do just this but requires you to use the NTPacket wrapper on both sides
	for (unsigned int i = 0; i < p.getDataLength(); i++) message += p.getData()[i]; 
	printf("\n\nReceived %d bytes: %s\n", p.getDataLength(), message.c_str());
	interfacer->multicastPacket(&p, interfacer->ServersideEndpoints);
}

int main(int argc, char* argv[])
{
	uint8_t addr[] = { XXX,XXX,XXX,XXX };

	EZMP::Init();

	for (int i = 0; i < argc; ++i)
		std::cout << argv[i] << "\n";

	if (strcmp(argv[1], "server") == 0) // if the command line argument inputs server
	{
		std::string pass;
		std::getline(std::cin, pass); // getting the password to generate the private encryption key

		interfacer = new MPInterfacer(123, (uint16_t)40007, addr, true, pass);
		interfacer->attachReceiveCallback(&onPacketReceive);
		printf("server mode");
		std::getchar();
	}
	if (strcmp(argv[1], "client") == 0) // if the command line argument inputs client
	{
		std::string pass;
		std::getline(std::cin, pass); // getting the password to generate the private encryption key

		interfacer = new MPInterfacer(123, (uint16_t)40007, addr, false, pass); // initializing an Interfacer
		printf("client mode\n");
		interfacer->attachReceiveCallback(&onPacketReceive); // attaching the callback that is called once a packet has been received

		Packet* send;
		while (true)
		{
			send = new Packet(false, false, false, 11, 1);
      // initializing the packet that isnt multicastable, isnt encrypted and doesnt repeat when hasn't been ACK by endpoint
			std::string toSend;
			std::getline(std::cin, toSend); // getting the string to send
			send->appendData((uint8_t*)toSend.c_str(), toSend.size(), PACKET_STRING); // appending the data from the string to the packet
			interfacer->sendPacket(send); // sending the packet via the interfacer
		}
	}
}
```
