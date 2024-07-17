#pragma once
#include "TimerThread.h"

using namespace std::chrono;

void Timer::Init(HANDLE h_cp)
{
	mHiocp = h_cp;

	std::priority_queue<std::vector<uint8_t>> empty_queue;
	std::swap(mTimerQueue, empty_queue);

	mIsRun = TRUE;
}

void Timer::PushEvent(std::vector<uint8_t>& buffer)
{
	std::lock_guard <std::mutex> lg{ mTimerLock };

	mTimerQueue.push(buffer);
}

void Timer::Main()
{
	while (mIsRun) {
		mTimerLock.lock();
		if (mTimerQueue.empty() == FALSE) {
			std::vector<uint8_t> top = mTimerQueue.top();
			int headerSize = sizeof(EVENT_HEADER);
			EVENT_HEADER* header = reinterpret_cast<EVENT_HEADER*>(top.data());
			if (header->start_time <= system_clock::now()) {
				mTimerQueue.pop();
				mTimerLock.unlock();
				ExOver* over_ex = new ExOver;
				over_ex->mOpType = eOpType::OP_EVENT;
				memcpy(over_ex->mMessageBuf, top.data(), top.size());
				PostQueuedCompletionStatus(mHiocp, 1, SERVERKEY, (LPOVERLAPPED)(&over_ex->mOver));
			}
			else {
				mTimerLock.unlock();
				std::this_thread::sleep_for(100ms);
			}
		}
		else {
			mTimerLock.unlock();
			std::this_thread::sleep_for(100ms);
		}
	}
}
