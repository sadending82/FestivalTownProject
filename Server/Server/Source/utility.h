#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include "protocol.h"

#define TIMEPOINT std::chrono::system_clock::time_point

#define COUT std::cout
#define ENDL std::endl;

inline constexpr int SERVERKEY = 0;
inline constexpr int STARTKEY = 1;
inline constexpr int INVALIDKEY = -1;
inline constexpr int TESTROOM = 0;

inline constexpr int BUFSIZE = 1000;

inline constexpr int MAXSESSION = 1000;
inline constexpr int MAXROOM = 100;
inline constexpr int MAXPLAYER = 6;
inline constexpr int MINPLAYER = 2;
inline constexpr int MAXOBJECT = 100;

inline constexpr int BLOCKSIZE = 2; // meter

inline constexpr float offsetX = 20;
inline constexpr float offsetZ = 10;

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

#define RunTest

enum class eSessionState
{
	ST_FREE, ST_ACCEPTED, ST_MATCHWAITING, ST_INGAME
};

enum class ePlayerState {
	PS_NONE,
	PS_ALIVE,
	PS_DEAD,
	PS_GROGGY
};

enum class eRoomState {
	RS_FREE,
	RS_INGAME
};

enum class TeamCode {
	RED,
	BLUE
};

enum class MapCode {
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

	bool operator == (const Vector3f& other) const {
		if (x == other.x && y == other.y && z == other.z) {
			return true;
		}
		return false;
	}

	bool operator < (const Vector3f& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return z < other.z;
	}
};

struct sPlayerGameRecord {
	int team;
	int kill_count;
	int death_count;
	int bomb_insert_count;
	int earn_gold;
	int point;
	int is_mvp;

	sPlayerGameRecord() : team(0), kill_count(0), death_count(0), bomb_insert_count(0), earn_gold(0), point(0), is_mvp(false) {};

	void Init() {
		team = 0;
		kill_count = 0;
		death_count = 0;
		bomb_insert_count = 0;
		earn_gold = 0;
		point = 0;
		is_mvp = false;
	}
};

#pragma pack(pop)

std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size);

Vector3f ConvertVec3fToVec2i(Vector3f vec);
Vector3f ConvertVec2iToVec3f(int x, int z);

struct MatchingCompare {
	bool operator()(class Player* a, class Player* b);
};

GameMode CulculateGameMode(int playerCount);