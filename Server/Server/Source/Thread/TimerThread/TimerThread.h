#pragma once
#include "../../Network/ExOver.h"
#include "../../PacketManager/PacketProcessors/Processors.h"
#include <queue>

struct Timer_event {
	int key;
	std::chrono::system_clock::time_point start_time;
	eOpType OpType;
	char buf[BUFSIZE + 1];

	constexpr bool operator< (const Timer_event& other) const {
		return start_time > other.start_time;
	}
};

class Timer {
public:
	std::priority_queue<Timer_event> mTimerQueue;

	void Init(HANDLE h_cp);
	void PushEvent(int key, eOpType event_type, int delaytime_ms, std::vector<uint8_t>& message);
	void Main();

	void Set_isRun(bool value) { mIsRun = value; }

private:
	int mIsRun;
	HANDLE mHiocp;
	std::mutex mTimerLock;
};