#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <random>
#include <string>
#include <locale.h>
#include <cctype>
#include <memory>

#define _WINSOCKAPI_
#include <windows.h>  
#include <winsock2.h>
#include <mswsock.h>
#undef max
#undef min

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

constexpr int SERVERKEY = 0;
constexpr int STARTKEY = 1;
constexpr int INVALIDKEY = -1;
constexpr int BUFSIZE = 1000;
constexpr int MAXSESSION = 1000;
constexpr int MAXROOM = 100;
constexpr int MAXPLAYER = 6;

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

enum ePacketType {
	// Server -> Client
	S2C_PLAYERADD,
	S2C_PLAYERGAMEINFO,
	S2C_PLAYERMOVE,
	S2C_PLAYERSTOP,

	// Client -> Server
	C2S_PLAYERMOVE,
	C2S_PLAYERSTOP,
};

enum eSessionState
{
	ST_FREE, ST_ACCEPTED, ST_INGAME
};

enum ePlayerState {
	ST_IDLE,
	ST_RUNNING,
	ST_WALKING,
	ST_DEAD
};

#pragma pack (push, 1)

struct HEADER {
public:
	uint16_t	size;
	uint16_t	type;
};

struct EVENT_HEADER {
	uint16_t	size;
	uint16_t	type;
	std::chrono::system_clock::time_point start_time;

	constexpr bool operator< (const EVENT_HEADER& other) const {
		return start_time > other.start_time;
	}
};

#pragma pack(pop)

std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size);

std::vector<uint8_t> MakeEventBuffer(const int type, const uint8_t* data, const int size, const int delaytime_ms);