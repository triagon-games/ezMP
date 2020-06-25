#include "MPInterfacer.h"
uint64_t uuid;

uint64_t count_digit(uint64_t number)
{
	return uint64_t(log10((double)number) + 1);
}

int iError;
WSADATA wsaData;
SOCKET recvSock;
sockaddr_in listenerSocket;
sockaddr_in incomingSocket;
uint8_t recvBuffer[RECEIVE_BUFFER_LEN];
int clientLength;

SOCKET sendSock;
sockaddr_in senderSocket;

uint8_t* sendFromAddr;
uint16_t sendToPort;
uint16_t listenOnPort;

MPInterfacer::MPInterfacer(uint64_t ClientUUID, std::string password, uint16_t sendPort, uint8_t* address, uint16_t recvPort)
{
	sendFromAddr = address;
	sendToPort = sendPort;
	listenOnPort = recvPort;

	uuid = ClientUUID;

	iError = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iError != 0)
	{
		printf("%s function failed when creating WSADATA line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to create WSADATA");
	}
	recvSock = socket(AF_INET, SOCK_DGRAM, 0);
	listenerSocket.sin_addr.S_un.S_addr = ADDR_ANY; // in this context, the server is the listenerm the client is the sender ... in multiplayer games the client must send and receive packets, so does the server
	listenerSocket.sin_family = AF_INET; // for this reason both the winsock2 server (listener) and winsock2 client (sender) must be created and initialized
	listenerSocket.sin_port = htons(recvPort); // little to big endian conversion

	iError = bind(recvSock, (sockaddr*)&listenerSocket, sizeof(listenerSocket));
	if (iError != 0)
	{
		printf("%s function failed binding SOCKET line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to bind SOCKET");
	}

	clientLength = sizeof(incomingSocket);
	ZeroMemory(&incomingSocket, clientLength); // allocating space for the client information

	ZeroMemory(recvBuffer, RECEIVE_BUFFER_LEN); // allocating space for the receiving buffer

	std::string ip;
	senderSocket.sin_family = AF_INET; // ipv4
	senderSocket.sin_port = htons(sendPort); // little to big endian conversion
	for (int i = 0; i < 3; i++) ip += std::to_string(address[i]) + '.';
	ip += std::to_string(address[3]);
	iError = inet_pton(AF_INET, (PSTR)ip.c_str(), &senderSocket.sin_addr);
	if (!iError)
	{
		printf("%s function failed parsing IP: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to parse IP");
	}

	sendSock = socket(AF_INET, SOCK_DGRAM, 0);
	m_LatencyCallback = NULL;
	m_ReceiveCallback = NULL;

	ListenerThread = std::thread(&MPInterfacer::ListenerFunction, this);
	ACKManagerThread = std::thread(&MPInterfacer::ACKManager, this);
}

MPInterfacer::~MPInterfacer()
{
	/*
	ListenerThread.~thread();
	ACKManagerThread.~thread();
	closesocket(recvSock);
	closesocket(sendSock);
	WSACleanup();
	*/
}

Packet MPInterfacer::recvPacket()
{
	ZeroMemory(recvBuffer, RECEIVE_BUFFER_LEN);
	int incomingBytes = recvfrom(recvSock, (char*)recvBuffer, RECEIVE_BUFFER_LEN, 0, (sockaddr*)&incomingSocket, & clientLength); // reading incoming packets
	if (incomingBytes == SOCKET_ERROR)
	{
		printf("Either an error has occured or there are no available packets: %d", WSAGetLastError());
	}
	if (incomingBytes != 0 && incomingBytes != SOCKET_ERROR)
	{
		//parsing the header
		uint16_t headerLen = recvBuffer[7] << 8 | recvBuffer[8];

		uint32_t payloadStart = ((uint32_t)recvBuffer[10] << 24) | ((uint32_t)recvBuffer[11] << 16) | ((uint32_t)recvBuffer[12] << 8 )| ((uint32_t)recvBuffer[13]); // p)arsing where the payload begins
		uint32_t payloadLen = ((uint32_t)recvBuffer[14] << 24) | ((uint32_t)recvBuffer[15] << 16) | ((uint32_t)recvBuffer[16] << 8 )| ((uint32_t)recvBuffer[17]); // p)arsing how long the payload is

		uint32_t metaStart = ((uint32_t)recvBuffer[18] << 24) | ((uint32_t)recvBuffer[19] << 16) | ((uint32_t)recvBuffer[20] << 8) | ((uint32_t)recvBuffer[21]); // parsing the meta start
		uint32_t metaLen = ((uint32_t)recvBuffer[22] << 24) | ((uint32_t)recvBuffer[23] << 16) | ((uint32_t)recvBuffer[24] << 8 )| ((uint32_t)recvBuffer[25]); // p)arsing the meta length

		bool ordered = (recvBuffer[4] >= 0 ? recvBuffer[4] : false);
		bool encrypted = (recvBuffer[5] >= 0 ? recvBuffer[5] : false);
		bool awaitACK = (recvBuffer[6] >= 0 ? recvBuffer[6] : false);
		uint32_t packetNum = ((uint32_t)recvBuffer[0] << 24) | ((uint32_t)recvBuffer[1] << 16) | ((uint32_t)recvBuffer[2] << 8) | ((uint32_t)recvBuffer[3]); // parsing the index of the packet

		Packet incoming = Packet(ordered, encrypted, awaitACK, recvBuffer[7], packetNum); // basically mocking a packet that is received ... makes it easier to deserialize and interpret data

		uint8_t* header = (uint8_t*)malloc(headerLen); // creating and extracting the header from the incoming bytes
		if (header == nullptr) throw std::runtime_error("incoming packet header initialization failed");
		memcpy(header, recvBuffer, headerLen);

		uint8_t* payload = (uint8_t*)malloc(payloadLen); // creating and extracting the payload from the incoming bytes
		if (payload == nullptr) throw std::runtime_error("incoming packet payload initialization failed");
		memcpy(payload, recvBuffer + payloadStart, payloadLen);

		uint8_t* meta = (uint8_t*)malloc(metaLen); // creating and extracting the payload from he incoming bytes
		if (meta == nullptr) throw std::runtime_error("incoming packet meta initialization failed");
		memcpy(meta, recvBuffer + metaStart, metaLen);

		incoming.setCompleteData(header, headerLen, payload, payloadLen, meta, metaLen); // shoving the data into the packet

		incoming.sourceAddr[0] = listenerSocket.sin_addr.S_un.S_un_b.s_b1; // setting the ip address of the source that came in from the client object on receive function call
		incoming.sourceAddr[1] = listenerSocket.sin_addr.S_un.S_un_b.s_b2; // seriously noit sure if this will work.
		incoming.sourceAddr[2] = listenerSocket.sin_addr.S_un.S_un_b.s_b3;
		incoming.sourceAddr[3] = listenerSocket.sin_addr.S_un.S_un_b.s_b4;

		return incoming;
	}
	return Packet();
}

void MPInterfacer::attachReceiveCallback(void (*func)(Packet))
{
	m_ReceiveCallback = func; // attach receive callback
}

void MPInterfacer::attachLatencyCallback(void (*func)(uint16_t))
{
	m_LatencyCallback = func; // attach latency callback
}

bool MPInterfacer::awaitPacket()
{
	return false;
}

Packet MPInterfacer::encryptPacket(Packet pkt)
{
	return pkt;
}

void MPInterfacer::startHandshake()
{
	Packet* init = new Packet(false, false, true, HANDSHAKE_PACKET, 0); // will initialize the key exchange sequence
	publicKey = generatePublicSecret(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	init->appendData(power(publicKey, privateKey, SECURE_PRIME_NUMBER));
	sendPacket(init); // send personal key
}

void MPInterfacer::sendPacket(Packet* pkt, bool retry)
{
	memcpy(pkt->sourceAddr, sendFromAddr, 4); // paste the source data into the packet object
	memcpy(&pkt->sourcePort, &sendToPort, sizeof(uint16_t)); // this data wont be sent anyways but who cares

	char* fullPacket = (char*)&(pkt->getFullPacket()[0]);

	unsigned int pktlen = pkt->getFullPacketLength();
	
	iError = sendto(sendSock, fullPacket, pktlen, 0, (sockaddr*)&senderSocket, sizeof(senderSocket)); // sending the packet and getting a result code to see if it failed
	if (iError == 0 || iError == -1)
	{
		printf("%s function failed when sending a packet line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to send packet over from SOCKET");
	}
	pkt->timeSent = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); // packet time of sending
	if (pkt->isAwaitACK() && !retry) // if the packet is awaitable, add it to the buffer to be checked and reset if timed out
	{
		ACKBuffer.push_back(pkt);
	}
}

uint64_t MPInterfacer::generatePublicSecret(uint64_t referenceMillis)
{
	srand((uint32_t)(referenceMillis % UINT32_MAX));
	return std::hash<long>{}(referenceMillis);
}

uint64_t MPInterfacer::generatePrivateSecret(std::string password)
{
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	uint64_t milliseconds = _byteswap_uint64(ms.count());

	uint8_t* m_ms = (uint8_t*)&milliseconds;

	uint32_t salt = (m_ms[0] ^ m_ms[1]) << 24 | (m_ms[2] ^ m_ms[3]) << 16 | (m_ms[4] ^ m_ms[5]) << 8 | (m_ms[6] ^ m_ms[7]);

	uint64_t maxMinNumber = 0;

	for (int i = 0; i < count_digit(UINT64_MAX); i++)
	{
		maxMinNumber += (uint64_t)(10, i);
	}
	srand((uint32_t)(milliseconds % UINT32_MAX));
	uint64_t privateKey = (rand() + maxMinNumber) % UINT64_MAX;
	privateKey = privateKey ^ uuid;

	privateKey = (((uint64_t)(((uint32_t*)&privateKey)[0] ^ salt)) << 32) | (((uint32_t*)&privateKey)[1] ^ salt);

	size_t passHash = std::hash<std::string>{}(password);
	size_t keyHash = std::hash<long>{}(privateKey);

	return keyHash ^ passHash;
}

void MPInterfacer::onHandshakeReceive(uint64_t secret, uint32_t exchangeNum, uint64_t referenceTime)
{
	sharedSecret = power(secret, privateKey, publicKey); // calculate the shared secret ... SHOULD be the same as on other side
	if (exchangeNum == 0)
	{
		publicKey = generatePublicSecret(referenceTime);
		Packet* followUp = new Packet(false, false, true, HANDSHAKE_PACKET, 1); // will initialize the key exchange sequence
		followUp->appendData(power(publicKey, privateKey, SECURE_PRIME_NUMBER));
		sendPacket(followUp); // send personal key
	}
}


void MPInterfacer::ListenerFunction() // will run continuously, invoking callbacks and analyzing the incoming data
{
	while (true)
	{
#ifdef LISTEN_INTERVAL
		std::this_thread::sleep_for(std::chrono::milliseconds(LISTEN_INTERVAL));
#endif
		Packet incoming = recvPacket();
		switch (incoming.getPacketType())
		{
		case LATENCY_PACKET:
		{
			m_LatencyCallback(incoming.get16AtLocation(0));
			break;
		}
		case HANDSHAKE_PACKET:
		{
			onHandshakeReceive(incoming.get64AtLocation(0), incoming.getPacketNum(), incoming.get64AtLocation(4)); // finishing the handshake 
			break;
		}
		case ACK_RESPONSE:
		{
			for (int i = 0; i < ACKBuffer.size(); i++)
			{
				if (ACKBuffer[i]->getPacketNum() == incoming.getPacketNum())
				{
					ACKBuffer.erase(ACKBuffer.begin() + i); // remove the packet from the buffer that we got a reply to
					ACKBuffer[i]->Deliver();
				}
			}
			break;
		}
		default:
		{
			if (incoming.isAwaitACK()) // if the incoming packet is awaiting an ACK reply, send one right away ... should contain NULL data bytes, only the header
			{
				Packet ACK_REPLY = Packet(false, false, false, ACK_RESPONSE, incoming.getPacketNum());
				sendPacket(&ACK_REPLY);
			}
			m_ReceiveCallback(incoming);
			break;
		}
		}
	}
}

void MPInterfacer::ACKManager()
{
	while (true)
	{
#ifdef ACK_INTERVAL
		std::this_thread::sleep_for(std::chrono::milliseconds(ACK_INTERVAL));
#endif
		uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		for (int i = 0; i < ACKBufferLength; i++)
		{
			if (time - ACKBuffer[i]->timeSent >= ACK_RETRY)
			{
				sendPacket(ACKBuffer[i], true);
			}
			if (time - ACKBuffer[i]->timeSent >= ACK_TIMEOUT)
			{
				ACKBuffer.erase(ACKBuffer.begin() + i);
			}
		}
	}
}