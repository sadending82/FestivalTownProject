#pragma once
#include "../utility.h"

class Timer;

#pragma pack (push, 1)
enum eEventType {
	HEARTBEAT,
	PLAYERPOSSYNC
};

struct EVENT_HEADER {
	char type;
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

struct EV_PLAYER_POS_SYNC : EVENT{
	int roodID;
};

struct EV_HEART_BEAT : EVENT {
	int sessionID;
};
#pragma pack(pop)

bool PushEventPlayerPosSync(Timer* pTimer, int roomID);
bool PushEventHeartBeat(Timer* pTimer, int sessionID);