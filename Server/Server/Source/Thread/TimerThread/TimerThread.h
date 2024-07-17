#pragma once
#include "../../Network/ExOver.h"
#include "../../PacketManager/PacketProcessors/Processors.h"
#include <queue>

class Timer {
public:
	std::priority_queue<std::vector<uint8_t>> mTimerQueue;

	void Init(HANDLE h_cp);
	void PushEvent(std::vector<uint8_t>& buffer);
	void Main();

	void Set_isRun(bool value) { mIsRun = value; }

private:
	int mIsRun;
	HANDLE mHiocp;
	std::mutex mTimerLock;
};