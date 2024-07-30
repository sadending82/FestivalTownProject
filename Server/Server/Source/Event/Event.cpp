#pragma once
#include "Event.h"
#include "../Thread/TimerThread/TimerThread.h"

bool PushEventPlayerPosSync(Timer* pTimer, int roomID)
{
	EV_PLAYER_POS_SYNC e;
	e.size = sizeof(EV_PLAYER_POS_SYNC);
	e.type = eEventType::PLAYERPOSSYNC;
	e.roodID = roomID;

	EVENT_HEADER header;
	header.type = eEventType::PLAYERPOSSYNC;
	header.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(6000);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_PLAYER_POS_SYNC));

	pTimer->PushEvent(header);

	return true;
}
