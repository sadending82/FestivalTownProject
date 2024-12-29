#pragma once
#include "TimerThread.h"

using namespace std::chrono;

void Timer::Init(HANDLE h_cp)
{
	mHiocp = h_cp;

	std::priority_queue<EVENT_HEADER> empty_queue;
	std::swap(mTimerQueue, empty_queue);

	mIsRun = TRUE;
}

void Timer::PushEvent(EVENT_HEADER event)
{
	mTimerLock.lock();
	mTimerQueue.push(event);
	mTimerLock.unlock();
}

void Timer::Main()
{
	while (mIsRun) {
		mTimerLock.lock();
		if (mTimerQueue.empty() == FALSE) {
			if (mTimerQueue.top().start_time <= system_clock::now()) {
				EVENT_HEADER eh = mTimerQueue.top();
				mTimerQueue.pop();
				mTimerLock.unlock();

				ExOver* over_ex = new ExOver;
				over_ex->mOpType = eOpType::OP_EVENT;
				memcpy(over_ex->mMessageBuf, eh.message, eh.message[0]);

				PostQueuedCompletionStatus(mHiocp, 1, SERVERKEY, (LPOVERLAPPED)(&over_ex->mOver));
			}
			else {
				mTimerLock.unlock();
				//std::this_thread::sleep_for(1ms);
			}
		}
		else {
			mTimerLock.unlock();
			//std::this_thread::sleep_for(1ms);
		}
	}
}
