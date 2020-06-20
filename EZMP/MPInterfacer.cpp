#include "MPInterfacer.h"
uint64_t uuid;

uint64_t count_digit(uint64_t number)
{
	return uint64_t(log10((double)number) + 1);
}

int iError;
WSADATA wsaData;
SOCKET recvSock;
sockaddr_in client;
uint8_t recvBuffer[RECEIVE_BUFFER_LEN];
int clientLength;

SOCKET sendSock;

uint8_t* sendFromAddr;
uint16_t sendToPort;
uint16_t listenOnPort;

MPInterfacer::MPInterfacer(uint64_t ClientUUID, char* password, uint16_t sendPort, uint8_t* address, uint16_t recvPort)
{
	sendFromAddr = address;
	sendToPort = sendPort;
	listenOnPort = recvPort;

	uuid = ClientUUID;
	publicSecret = generatePublicSecret(NULL);

	//TODO: SERGEI FINISH SENDING PACKET
	iError = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iError != 0)
	{
		printf("%s function failed when creating WSADATA line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to create WSADATA");
	}
	recvSock = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in listenerSocket; // just fill out necessary information // we are starting to LISTEN for packets
	listenerSocket.sin_addr.S_un.S_addr = ADDR_ANY; // in this context, the server is the listenerm the client is the sender ... in multiplayer games the client must send and receive packets, so does the server
	listenerSocket.sin_family = AF_INET; // for this reason both the winsock2 server (listener) and winsock2 client (sender) must be created and initialized
	listenerSocket.sin_port = htons(recvPort); // little to big endian conversion

	iError = bind(recvSock, (sockaddr*)&listenerSocket, sizeof(listenerSocket));
	if (iError != 0)
	{
		printf("%s function failed binding SOCKET line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to bind SOCKET");
	}

	clientLength = sizeof(client);
	ZeroMemory(&client, clientLength); // allocating space for the client information

	ZeroMemory(recvBuffer, RECEIVE_BUFFER_LEN); // allocating space for the receiving buffer

	std::string ip;
	sockaddr_in senderSocket; // creating a socket that will SEND packets
	senderSocket.sin_family = AF_INET; // ipv4
	senderSocket.sin_port = htons(sendPort); // little to big endian conversion
	for (int i = 0; i < 3; i++) ip += std::to_string(address[i]) + '.';
	ip += std::to_string(address[3]);
	
	IN_ADDR dstAddr;
	inet_ntop(AF_INET, &dstAddr, (PSTR)ip.c_str(), ip.length);
	senderSocket.sin_addr = dstAddr;

	sendSock = socket(AF_INET, SOCK_DGRAM, 0);
}

MPInterfacer::~MPInterfacer()
{
	closesocket(recvSock);
	closesocket(sendSock);
	WSACleanup();
}

Packet MPInterfacer::recvPacket()
{
	ZeroMemory(recvBuffer, RECEIVE_BUFFER_LEN);
	uint32_t incomingBytes = recvfrom(recvSock, (char*)recvBuffer, RECEIVE_BUFFER_LEN, 0, (sockaddr*)&client, &clientLength); // reading incoming packets
	if (incomingBytes != 0)
	{
		printf("Either an error has occured or there are no available packets: %d", WSAGetLastError());
	}
	//parsing the header
	uint16_t headerLen = recvBuffer[7] << 8 | recvBuffer[8];

	uint32_t payloadStart = recvBuffer[10] << 24 | recvBuffer[11] << 16 | recvBuffer[12] << 8 | recvBuffer[13];
	uint32_t payloadLen = recvBuffer[14] << 24 | recvBuffer[15] << 16 | recvBuffer[16] << 8 | recvBuffer[17];

	uint32_t metaStart = recvBuffer[18] << 24 | recvBuffer[19] << 16 | recvBuffer[20] << 8 | recvBuffer[21];
	uint32_t metaLen = recvBuffer[22] << 24 | recvBuffer[23] << 16 | recvBuffer[24] << 8 | recvBuffer[25];

	bool ordered = (recvBuffer[4] >= 0 ? recvBuffer[4] : false);
	bool encrypted = (recvBuffer[5] >= 0 ? recvBuffer[5] : false);
	bool awaitACK = (recvBuffer[6] >= 0 ? recvBuffer[6] : false);
	uint32_t packetNum = recvBuffer[0] << 24 | recvBuffer[1] << 16 | recvBuffer[2] << 8 | recvBuffer[3];

	Packet incoming = Packet(incomingBytes, ordered, encrypted, awaitACK, recvBuffer[7], packetNum); // basically mocking a packet that is received ... makes it easier to deserialize and interpret data

	uint8_t* header = (uint8_t*)malloc(headerLen); // creating and extracting the header from the incoming bytes
	if (header == nullptr) throw std::runtime_error("incoming packet header initialization failed");
	memcpy(header, recvBuffer, headerLen);

	uint8_t* payload = (uint8_t*)malloc(payloadLen); // creating and extracting the payload from the incoming bytes
	if (payload == nullptr) throw std::runtime_error("incoming packet payload initialization failed");
	memcpy(payload, recvBuffer+payloadStart, payloadLen);

	uint8_t* meta = (uint8_t*)malloc(metaLen); // creating and extracting the payload from he incoming bytes
	if (meta == nullptr) throw std::runtime_error("incoming packet meta initialization failed");
	memcpy(meta, recvBuffer+metaStart, metaLen);

	incoming.setCompleteData(header, headerLen, payload, payloadLen, meta, metaLen); // shoving the data into the packet

	incoming.sourceAddr[0] = client.sin_addr.S_un.S_un_b.s_b1; // setting the ip address of the source that came in from the client object on receive function call
	incoming.sourceAddr[1] = client.sin_addr.S_un.S_un_b.s_b2;
	incoming.sourceAddr[2] = client.sin_addr.S_un.S_un_b.s_b3;
	incoming.sourceAddr[3] = client.sin_addr.S_un.S_un_b.s_b4;
	
	return incoming;
}

bool MPInterfacer::awaitPacket()
{
	return false;
}

Packet MPInterfacer::encryptPacket(Packet pkt)
{
	return pkt;
}

void MPInterfacer::sendPacket(Packet pkt)
{
	memcpy(pkt.sourceAddr, sendFromAddr, 4); // paste the source data into the packet object
	memcpy(&pkt.sourcePort, &sendToPort, sizeof(uint16_t)); // this data wont be sent anyways but who cares

	char* fullPacket = (char*)pkt.getFullPacket();

	iError = send(sendSock, fullPacket, pkt.getFullPacketLength(), 0); // sending the packet and getting a result code to see if it failed
	if (iError != 0)
	{
		printf("%s function failed when sending a packet line: %d\n error: %d", __func__, __LINE__, iError);
		throw std::runtime_error("unable to send packet over from SOCKET");
	}
}

uint64_t MPInterfacer::generatePublicSecret(uint64_t referenceMillis)
{
	return std::hash<long>{}(referenceMillis);
}

uint64_t MPInterfacer::generatePrivateSecret(char* password)
{
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	uint64_t milliseconds = _byteswap_uint64(ms.count());
	delete& ms, sizeof(std::chrono::milliseconds);

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

	size_t passHash = std::hash<char*>{}(password);
	size_t keyHash = std::hash<long>{}(privateKey);

	delete& milliseconds;
	return keyHash ^ passHash;
}