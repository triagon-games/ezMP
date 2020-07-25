#pragma once
#include "DLLDefinitions.h"

#define RECEIVE_BUFFER_LEN 1024

#define __LIB_VER_MAJOR__ 0
#define __LIB_VER_MINOR__ 5
#define __LIB_VER_PATCH__ 13

#define DEBUG_VERBOSITY_LEVEL 1

#ifndef __GAME_VER__
	#define __GAME_VER__ 1
#endif

#ifndef SECURE_PRIME_NUMBER
	#define SECURE_PRIME_NUMBER 957317
#endif

#ifndef ACK_TIMEOUT
	#define ACK_TIMEOUT 5000
#endif

#ifndef ACK_RETRY
	#define ACK_RETRY 500
#endif

#ifndef PUBLIC_STUN_SERVER
	#define PUBLIC_STUN_SERVER "stun.callwithus.com"
#endif

#define LATENCY_PACKET 0
#define LOGIN_PACKET 1
#define NETWORK_TABLE_PACKET 11
#define ACK_RESPONSE 255
#define HANDSHAKE_PACKET 100
#define HIDDEN_PACKET 101
#define SERVICE_PACKET 102
#define UNSPECIFIED_PACKET 0b11111111

#define AES_INSTRUCTION_SET

#define PACKET_FLOAT 1
#define PACKET_DOUBLE 2
#define PACKET_BYTE 3
#define PACKET_U16 4
#define PACKET_U32 5
#define PACKET_U64 6
#define PACKET_STRING 7
#define PACKET_VARIABLE_NAME 200
#define PACKET_ENUMERATOR 201