#include "MPInterfacer.h"
uint64_t uuid;

uint64_t count_digit(uint64_t number)
{
	return uint64_t(log10((double)number) + 1);
}

int iError;
WSADATA wsaData;
SOCKET m_Socket;
sockaddr_in m_SocketAddress;
sockaddr_in m_ListenSocketAddress;
sockaddr_in incomingSocket;
uint8_t recvBuffer[RECEIVE_BUFFER_LEN];
int clientLength;

uint8_t* sendFromAddr;
uint16_t sendToPort;
uint16_t listenOnPort;

MPInterfacer::MPInterfacer(uint64_t ClientUUID, uint16_t Port, uint8_t* address, bool isServer, std::string Pass)
{
	whatAmI.IP = Utils::getStringFromIP(Utils::getPublicIPAddress());
	whatAmI.portPair = Utils::getPortTranslation(Port);

	sendFromAddr = address;
	sendToPort = Port;
	listenOnPort = Port;

	uuid = ClientUUID;

	iError = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iError != 0)
	{
		printf("%s function failed when creating WSADATA line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to create WSADATA");
	}

	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	std::string ip;
	for (int i = 0; i < 3; i++) ip += std::to_string(address[i]) + '.';
	ip += std::to_string(address[3]);
	iError = inet_pton(AF_INET, (PSTR)ip.c_str(), &m_SocketAddress.sin_addr);
	m_SocketAddress.sin_family = AF_INET; // for this reason both the winsock2 server (listener) and winsock2 client (sender) must be created and initialized
	m_SocketAddress.sin_port = htons(Port); // little to big endian conversion

	m_ListenSocketAddress.sin_addr.S_un.S_addr = ADDR_ANY;
	m_ListenSocketAddress.sin_family = AF_INET;
	m_ListenSocketAddress.sin_port = htons(Port);

	if (!isServer)
	{
		iError = bind(m_Socket, (sockaddr*)&m_ListenSocketAddress, sizeof(m_ListenSocketAddress)); // setting the outbound port
		if (iError != 0)
		{
			printf("%s function failed binding SOCKET line: %d\n error: %d", __func__, __LINE__, iError);
			throw std::runtime_error("unable to bind SOCKET");
		}
		HolePunch();
	}
	else
	{
#ifndef NO_UPNP
		try
		{
			//PortForwardEngine::UPnPportForward(Port, Port);
		}
		catch (std::exception ex) {}
#endif

		iError = bind(m_Socket, (sockaddr*)&m_ListenSocketAddress, sizeof(m_ListenSocketAddress)); // setting the inbound port
		if (iError != 0)
		{
			printf("%s function failed binding SOCKET line: %d\n error: %d", __func__, __LINE__, iError);
			throw std::runtime_error("unable to bind SOCKET");
		}
	}

	clientLength = sizeof(incomingSocket);
	ZeroMemory(&incomingSocket, clientLength); // allocating space for the client information

	ZeroMemory(recvBuffer, RECEIVE_BUFFER_LEN); // allocating space for the receiving buffer

	m_LatencyCallback = NULL;
	m_ReceiveCallback = NULL;

	ListenerThread = std::thread(&MPInterfacer::ListenerFunction, this);
	ACKManagerThread = std::thread(&MPInterfacer::ACKManager, this);

	privateKey = generatePrivateKey(Pass);
	whatAmI.privateKey = privateKey;

	if (!isServer)
	{
		startHandshake();
	}
	this->isServer = isServer;
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
	int incomingBytes = recvfrom(m_Socket, (char*)recvBuffer, RECEIVE_BUFFER_LEN, 0, (sockaddr*)&incomingSocket, & clientLength); // reading incoming packets
	if (incomingBytes == SOCKET_ERROR)
	{
		printf("Either an error has occured or there are no available packets: %d", WSAGetLastError());
	}
	if (incomingBytes != 0 && incomingBytes != SOCKET_ERROR)
	{
		if (!recvBuffer[8] == 26) return Packet();
		//parsing the header
		uint16_t headerLen = recvBuffer[8] << 8 | recvBuffer[7];

		uint32_t payloadStart = ((uint32_t)recvBuffer[10] << 24) | ((uint32_t)recvBuffer[11] << 16) | ((uint32_t)recvBuffer[12] << 8 )| ((uint32_t)recvBuffer[13]); // p)arsing where the payload begins
		uint32_t payloadLen = ((uint32_t)recvBuffer[14] << 24) | ((uint32_t)recvBuffer[15] << 16) | ((uint32_t)recvBuffer[16] << 8 )| ((uint32_t)recvBuffer[17]); // p)arsing how long the payload is

		uint32_t metaStart = ((uint32_t)recvBuffer[18] << 24) | ((uint32_t)recvBuffer[19] << 16) | ((uint32_t)recvBuffer[20] << 8) | ((uint32_t)recvBuffer[21]); // parsing the meta start
		uint32_t metaLen = ((uint32_t)recvBuffer[22] << 24) | ((uint32_t)recvBuffer[23] << 16) | ((uint32_t)recvBuffer[24] << 8 )| ((uint32_t)recvBuffer[25]); // p)arsing the meta length

		bool ordered = (recvBuffer[4] >= 0 ? recvBuffer[4] : false);
		bool encrypted = (recvBuffer[5] >= 0 ? recvBuffer[5] : false);
		bool awaitACK = (recvBuffer[6] >= 0 ? recvBuffer[6] : false);
		uint32_t packetNum = ((uint32_t)recvBuffer[0] << 24) | ((uint32_t)recvBuffer[1] << 16) | ((uint32_t)recvBuffer[2] << 8) | ((uint32_t)recvBuffer[3]); // parsing the index of the packet

		Packet incoming = Packet(ordered, encrypted, awaitACK, recvBuffer[7], packetNum); // basically mocking a packet that is received ... makes it easier to deserialize and interpret data

		std::vector<uint8_t> header;

		std::vector<uint8_t> payload;

		std::vector<uint8_t> meta;

		for (unsigned int i = 0; i < headerLen; i++)
		{
			header.push_back(recvBuffer[i]);
		}
		if (payloadLen != 0)
		{
			for (unsigned int i = payloadStart; i < payloadStart + payloadLen; i++)
			{
				payload.push_back(recvBuffer[i]);
			}
		}
		if (metaLen != 0)
		{
			for (unsigned int i = metaStart; i < metaStart + metaLen; i++)
			{
				meta.push_back(recvBuffer[i]);
			}
		}

		incoming.setCompleteData(header, headerLen, payload, payloadLen, meta, metaLen); // shoving the data into the packet

		incoming.source.IP = std::to_string(incomingSocket.sin_addr.S_un.S_un_b.s_b1) + '.'
			+ std::to_string(incomingSocket.sin_addr.S_un.S_un_b.s_b2) + '.'
			+ std::to_string(incomingSocket.sin_addr.S_un.S_un_b.s_b3) + '.'
			+ std::to_string(incomingSocket.sin_addr.S_un.S_un_b.s_b4);
		incoming.source.portPair.OutboundPublic = incomingSocket.sin_port;

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

void MPInterfacer::HolePunch()
{
	Packet* ack = new Packet(false, false, true, SERVICE_PACKET, 0);
	sendPacket(ack);
}

Packet MPInterfacer::encryptPacket(Packet pkt, SSLAES eContext)
{
	//uint8_t* encryptedData = SSLAES.
	return pkt;
}

void MPInterfacer::startHandshake()
{
	Packet* init = new Packet(false, false, true, HANDSHAKE_PACKET, 0); // will initialize the key exchange sequence
	uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	publicKey = generatePublicKey(time);
	uint64_t pwr = generateRuledKey(publicKey, privateKey, SECURE_PRIME_NUMBER);

	init->appendData(pwr);
	init->appendData(time);
	sendPacket(init); // send personal key
}

void MPInterfacer::sendPacket(Packet* pkt, bool retry)
{
	memcpy(pkt->sourceAddr, Utils::getIPFromString(whatAmI.IP), 4); // paste the source data into the packet object
	memcpy(&pkt->sourcePort, &sendToPort, sizeof(uint16_t)); // this data wont be sent anyways but who cares

	std::vector<uint8_t> packet = pkt->getFullPacket();
	char* fullPacket = (char*)&packet[0];

	unsigned int pktlen = pkt->getFullPacketLength();
	
	iError = sendto(m_Socket, fullPacket, pktlen, 0, (sockaddr*)&m_SocketAddress, sizeof(m_SocketAddress)); // sending the packet and getting a result code to see if it failed
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

void MPInterfacer::sendPacket(Packet* pkt, bool retry, Utils::Endpoint endpoint)
{
	sockaddr_in targetAddress;
	targetAddress.sin_family = AF_INET;
	targetAddress.sin_port = htons(endpoint.portPair.OutboundPublic);
	inet_pton(AF_INET, (PSTR)&endpoint.IP, &targetAddress.sin_addr);

	memcpy(pkt->sourceAddr, sendFromAddr, 4); // paste the source data into the packet object
	memcpy(&pkt->sourcePort, &localPortMapping.OutboundPublic, sizeof(uint16_t)); // this data wont be sent anyways but who cares

	std::vector<uint8_t> packet = pkt->getFullPacket();
	char* fullPacket = (char*)&packet[0];

	unsigned int pktlen = pkt->getFullPacketLength();

	iError = sendto(m_Socket, fullPacket, pktlen, 0, (sockaddr*)&m_SocketAddress, sizeof(m_SocketAddress)); // sending the packet and getting a result code to see if it failed
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

void MPInterfacer::multicastPacket(Packet* pkt, std::vector<Utils::Endpoint> group)
{

}

uint32_t MPInterfacer::generatePublicKey(uint64_t referenceMillis)
{
	srand((uint32_t)(referenceMillis % UINT32_MAX));
	return std::hash<long>{}(referenceMillis);
}

uint32_t MPInterfacer::generatePrivateKey(std::string password)
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

	uint64_t tmp = keyHash ^ passHash;

	return ((uint32_t*)&tmp)[0] ^ ((uint32_t*)&tmp)[1];
}

void MPInterfacer::onHandshakeReceive(uint32_t secret, uint32_t exchangeNum, uint64_t referenceTime, Utils::Endpoint source)
{
	if (exchangeNum == 0)
	{
		publicKey = generatePublicKey(referenceTime);
		Packet* followUp = new Packet(false, false, true, HANDSHAKE_PACKET, 1); // will initialize the key exchange sequence
		uint64_t pwr = generateRuledKey(privateKey, publicKey, SECURE_PRIME_NUMBER);
		followUp->appendData(pwr);
		sendPacket(followUp); // send personal key
	}
	sharedSecret = generateRuledKey(secret, privateKey, SECURE_PRIME_NUMBER); // calculate the shared secret ... SHOULD be the same as on other side
	SSLAES tmp((unsigned char*)&sharedSecret, sizeof(sharedSecret));
	if (isServer)
	{
		source.privateKey = sharedSecret;
		ServersideEndpoints.push_back(source);
		SubscriberAESContexts.push_back(tmp);
	}
	else
	{
		selfSSLContext = tmp;
	}
	printf("\nShared secret: %i", sharedSecret);
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
				uint64_t incomingTime = 0;
				if (!incoming.getPacketNum()) incomingTime = incoming.get64AtLocation(8);
				onHandshakeReceive(incoming.get64AtLocation(0), incoming.getPacketNum(), incomingTime, incoming.source); // finishing the handshake 
				break;
			}
			case ACK_RESPONSE:
			{
				for (int i = 0; i < ACKBuffer.size(); i++)
				{
					if (ACKBuffer[i]->getPacketNum() == incoming.getPacketNum())
					{
						ACKBuffer[i]->Deliver();
						ACKBuffer.erase(ACKBuffer.begin() + i); // remove the packet from the buffer that we got a reply to
						printf("ACK Received\n");
					}
				}
				break;
			}
			case LOGIN_PACKET:
			{

				break;
			}
			default:
			{
				if (incoming.isAwaitACK()) // if the incoming packet is awaiting an ACK reply, send one right away ... should contain NULL data bytes, only the header
				{
					Packet ACK_REPLY = Packet(false, false, false, ACK_RESPONSE, incoming.getPacketNum());
					sendPacket(&ACK_REPLY);
					printf("ACK Sent\n");
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