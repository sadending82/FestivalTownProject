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
#include <cmath>
#include <memory>
#include <mutex>
#include <shared_mutex>

#define _WINSOCKAPI_
#include <windows.h>  
#include <winsock2.h>
#include <mswsock.h>
#undef max
#undef min

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

#define TIMEPOINT std::chrono::system_clock::time_point

constexpr int SERVERKEY = 0;
constexpr int STARTKEY = 1;
constexpr int INVALIDKEY = -1;
constexpr int TESTROOM = 0;

constexpr int BUFSIZE = 1000;

constexpr int MAXSESSION = 1000;
constexpr int MAXROOM = 100;
constexpr int MAXPLAYER = 6;
constexpr int MINPLAYER = 2;
constexpr int MAXOBJECT = 100;

const uint8_t DEFAULT_8BITFLAG = 0b00000000;

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

#define RunTest

enum eSessionState
{
	ST_FREE, ST_ACCEPTED, ST_GAMEREADY, ST_INGAME
};

enum ePlayerState {
	PS_NONE,
	PS_ALIVE,
	PS_DEAD
};

enum eRoomState {
	RS_FREE,
	RS_INGAME
};

enum TeamCode {
	RED,
	BLUE
};

enum MapCode {
	TEST
};

#pragma pack (push, 1)

struct HEADER {
public:
	uint16_t	flatBufferSize;
	uint16_t	type;
};

struct Vector3f {
	float x;
	float y;
	float z;

	Vector3f() : x(0), y(0), z(0) {};
	Vector3f(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {};

	bool operator == (const Vector3f other) const {
		if (x == other.x && y == other.y && z == other.z) {
			return true;
		}
		return false;
	}
};

#pragma pack(pop)

std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size);