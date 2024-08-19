#pragma once
#include "../utility.h"

constexpr int HEARTBEATINTERVAL = 6000;
constexpr int BLOCKDROPINTERVAL = 10000;
constexpr int REMAINTIMESYNCINTERVAL = 30000;
constexpr int BOMBEXPLOSIONINTERVAL = 60000;
constexpr int TIMEOVERCHECKINTERVAL = 5000;

class Timer;

#pragma pack (push, 1)
enum eEventType {
	HEARTBEAT,
	
	// InGmae Event
	BLOCKDROP,
	BOMBSPAWN,
	BOMBEXPLOSION,
	TIMEOVERCHECK,

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


struct EV_HEART_BEAT : EVENT {
	int sessionID;
};

struct EV_OBJECT_DROP : EVENT {
	int roomID;
};

struct EV_BOMB_SPAWN : EVENT{
	int roomID;
};

struct EV_BOMB_EXPLOSION : EVENT {
	int roomID;
	int bombID;
};

struct EV_TIMEOVER_CHECK : EVENT {
	int roomID;
};

struct EV_TIME_SYNC : EVENT {
	int roomID;
	int time;
};

#pragma pack(pop)

bool PushEventHeartBeat(Timer* pTimer, int sessionID);

// InGameEvent
bool PushEventBlockDrop(Timer* pTimer, int roomID, int intervalTime);
bool PushEventBombSpawn(Timer* pTimer, int roomID, int intervalTime);
bool PushEventRemainTimeSync(Timer* pTimer, int roomID);
bool PushEventBombExplosion(Timer* pTimer, int roomID, int bombID);
bool PushEventTimeOverCheck(Timer* pTimer, int roomID);