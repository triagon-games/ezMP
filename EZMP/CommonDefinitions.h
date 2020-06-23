#pragma once

#define RECEIVE_BUFFER_LEN 1024

#define __LIB_VER__ 0.0.0

#ifndef __GAME_VER__
	#define __GAME_VER__ 0
#endif

#ifndef SECURE_PRIME_NUMBER
	#define SECURE_PRIME_NUMBER 0
#endif

#ifndef ACK_TIMEOUT
	#define ACK_TIMEOUT 1000
#endif

#define LATENCY_PACKET 0
#define ACK_RESPONSE 255
#define HANDSHAKE_PACKET 100
#define HIDDEN_PACKET 101
#define SERVICE_PACKET 102
#define UNSPECIFIED_PACKET 0b11111111