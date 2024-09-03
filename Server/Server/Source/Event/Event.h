#pragma once
#include "../utility.h"

// miliseconds
constexpr int HEARTBEATINTERVAL = 6000;
constexpr int BLOCKDROPINTERVAL = 10000;
constexpr int REMAINTIMESYNCINTERVAL = 30000;
constexpr int TIMEOVERCHECKINTERVAL = 5000;
constexpr int GAMESTARTINTERVAL = 4000;
constexpr int GAMEMATCHINGINTERVAL = 10000;

class Timer;

#pragma pack (push, 1)
enum eEventType {
	HEARTBEAT,
	GAMEMATCHING,

	// InGmae Event
	GAMESTART,
	BLOCKDROP,
	BOMBSPAWN,
	BOMBEXPLOSION,
	TIMEOVERCHECK,
	PLAYERRESPAWN,
	WEAPONSPAWN,

	TIMESYNC
};

struct EVENT_HEADER {
	std::chrono::system_clock::time_point start_time;

	constexpr bool operator< (const EVENT_HEADER& other) const {
		return start_time > other.start_time;
	}

	// 이벤트 세부 내용이 여기에 들어감
	char message[BUFSIZE + 1];
};

struct EVENT {
	char size;
	char type;
};

struct EV_GAME_MATCHING : EVENT {
	
};

struct EV_GAME_START : EVENT {
	int roomID;
	long long roomCode;
};

struct EV_HEART_BEAT : EVENT {
	int sessionID;
};

struct EV_OBJECT_DROP : EVENT {
	int roomID;
	long long roomCode;
};

struct EV_BOMB_SPAWN : EVENT{
	int roomID;
	long long roomCode;
};

struct EV_BOMB_EXPLOSION : EVENT {
	int roomID;
	int bombID;
	long long roomCode;
};

struct EV_TIMEOVER_CHECK : EVENT {
	int roomID;
	long long roomCode;
};

struct EV_TIME_SYNC : EVENT {
	int roomID;
	int time;
	long long roomCode;
};

struct EV_PLAYER_RESPAWN : EVENT {
	int playerID;
	int roomID;
	long long roomCode;
};

struct EV_WEAPON_SPAWN : EVENT {
	int roomID;
	long long roomCode;
};

#pragma pack(pop)

bool PushEventHeartBeat(Timer* pTimer, int sessionID);
bool PushEventGameMatching(Timer* pTimer);

// InGameEvent
bool PushEventGameStart(Timer* pTimer, int roomID, long long roomCode);
bool PushEventBlockDrop(Timer* pTimer, int roomID, long long roomCode, int intervalSecond);
bool PushEventBombSpawn(Timer* pTimer, int roomID, long long roomCode, int intervalSecond);
bool PushEventWeaponSpawn(Timer* pTimer, int roomID, long long roomCode, int intervalSecond);
bool PushEventRemainTimeSync(Timer* pTimer, int roomID, long long roomCode);
bool PushEventBombExplosion(Timer* pTimer, int roomID, int bombID, long long roomCode, int intervalSecond);
bool PushEventTimeOverCheck(Timer* pTimer, int roomID, long long roomCode);
bool PushEventPlayerRespawn(Timer* pTimer, int playerID, int roomID, long long roomCode, int intervalSecond);