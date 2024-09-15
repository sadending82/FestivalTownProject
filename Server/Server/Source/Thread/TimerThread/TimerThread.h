#pragma once
#include "../../Network/ExOver.h"
#include "../../Event/Event.h"
#include <queue>

class Timer {
public:
	std::priority_queue<EVENT_HEADER> mTimerQueue;

	void Init(HANDLE h_cp);
	void PushEvent(EVENT_HEADER event);
	void Main();

	void Set_isRun(bool value) { mIsRun = value; }

private:
	bool mIsRun = true;
	HANDLE mHiocp;
	std::mutex mTimerLock;
};