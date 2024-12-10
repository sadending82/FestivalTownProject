#pragma once
#include "../utility.h"

// miliseconds
inline constexpr int HEARTBEATINTERVAL = 60000;
inline constexpr int BLOCKDROPINTERVAL = 10000;
inline constexpr int REMAINTIMESYNCINTERVAL = 10000;
inline constexpr int TIMEOVERCHECKINTERVAL = 5000;
inline constexpr int GAMESTARTINTERVAL = 4000;
inline constexpr int GAMEMATCHINGINTERVAL = 60000;

inline constexpr float SECONDBLOCKSPAWNADDINTERVAL = 0.333f;

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

	GROGGYRECOVERY,

	TIMESYNC
};

struct EVENT_HEADER {
	std::chrono::system_clock::time_point start_time;

	inline constexpr bool operator< (const EVENT_HEADER& other) const {
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
	int requesterID;
	long long requestTime;
	eMatchingType matchingType;
};

struct EV_GAME_START : EVENT {
	int roomID;
	long long roomCode;
};

struct EV_HEART_BEAT : EVENT {
	int sessionID;
	long long authenticationKey;
};

struct EV_OBJECT_DROP : EVENT {
	int roomID;
	long long roomCode;
	int objectType;
};

struct EV_BOMB_SPAWN : EVENT{
	int roomID;
	long long roomCode;
};

struct EV_BOMB_EXPLOSION : EVENT {
	int roomID;
	int bombID;
	long long bombUniqueCode;
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
	int weaponType;
};

struct EV_GROGGY_RECOVERY : EVENT {
	int roomID;
	int playerID;
	long long roomCode;
};

#pragma pack(pop)

bool PushEventHeartBeat(Timer* pTimer, int sessionID, long long authenticationKey);
bool PushEventGameMatching(Timer* pTimer, int requesterID, eMatchingType matchingType, long long requestTime, int intervalSecond);

// InGameEvent
bool PushEventGameStart(Timer* pTimer, int roomID, long long roomCode);
bool PushEventBlockDrop(Timer* pTimer, int roomID, long long roomCode, int blockType, float intervalSecond);
bool PushEventBombSpawn(Timer* pTimer, int roomID, long long roomCode, int intervalSecond);
bool PushEventWeaponSpawn(Timer* pTimer, int roomID, long long roomCode, int weaponType, int intervalSecond);
bool PushEventRemainTimeSync(Timer* pTimer, int roomID, long long roomCode);
bool PushEventBombExplosion(Timer* pTimer, int roomID, int bombID, long long bombUniqueCode, long long roomCode, int intervalSecond);
bool PushEventTimeOverCheck(Timer* pTimer, int roomID, long long roomCode);
bool PushEventPlayerRespawn(Timer* pTimer, int playerID, int roomID, long long roomCode, int intervalSecond);
bool PushEventGroggyRecovery(Timer* pTimer, int playerID, int roomID, long long roomCode, int intervalSecond);