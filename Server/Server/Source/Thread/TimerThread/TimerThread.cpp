#pragma once
#include "TimerThread.h"

using namespace std::chrono;

void Timer::Init(HANDLE h_cp)
{
	mHiocp = h_cp;

	std::priority_queue<Timer_event> empty_queue;
	std::swap(mTimerQueue, empty_queue);

	mIsRun = TRUE;
}

void Timer::PushEvent(int key, eOpType event_type, int delaytime_ms, std::vector<uint8_t>& message)
{
	std::lock_guard <std::mutex> lg{ mTimerLock };
	Timer_event te;
	te.key = key;
	te.OpType = event_type;
	te.start_time = system_clock::now() + milliseconds(delaytime_ms);
	memcpy(te.buf, message.data(), message.size());
	mTimerQueue.push(te);
}

void Timer::Main()
{
	while (mIsRun) {
		mTimerLock.lock();
		if ((mTimerQueue.empty() == FALSE)
			&& (mTimerQueue.top().start_time <= system_clock::now())) {
			Timer_event te = mTimerQueue.top();
			mTimerQueue.pop();
			mTimerLock.unlock();

			ExOver* over_ex = new ExOver;
			over_ex->mTargetID = te.key;
			over_ex->mOpType = te.OpType;
			memcpy(over_ex->mMessageBuf, te.buf, sizeof(te.buf));

			PostQueuedCompletionStatus(mHiocp, 1, te.key, (LPOVERLAPPED)(&over_ex->mOver));
		}
		else {
			mTimerLock.unlock();
			std::this_thread::sleep_for(100ms);
		}
	}
}
