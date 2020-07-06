#pragma once

#define RECEIVE_BUFFER_LEN 1024

#define __LIB_VER_MAJOR__ 0
#define __LIB_VER_MINOR__ 1
#define __LIB_VER_PATCH__ 0

#ifndef __GAME_VER__
	#define __GAME_VER__ 0
#endif

#ifndef SECURE_PRIME_NUMBER
	#define SECURE_PRIME_NUMBER 0
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
#define ACK_RESPONSE 255
#define HANDSHAKE_PACKET 100
#define HIDDEN_PACKET 101
#define SERVICE_PACKET 102
#define UNSPECIFIED_PACKET 0b11111111