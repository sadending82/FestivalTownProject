#pragma once
#include "../utility.h"

constexpr int HEARTBEATTIME = 6000;

constexpr int OBJECTDROPTIME = 10000;

class Timer;

#pragma pack (push, 1)
enum eEventType {
	HEARTBEAT,
	
	// InGmae Event
	OBJECTDROP,
	BOMBSPAWN,
	TIMERSYNC
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

struct EV_TIMER_SYNC : EVENT {
	int roomID;
	int time;
};

#pragma pack(pop)

bool PushEventHeartBeat(Timer* pTimer, int sessionID);

// InGameEvent
bool PushEventObjectDrop(Timer* pTimer, int roomID, int time);
bool PushEventBombSpawn(Timer* pTimer, int roomID, int time);