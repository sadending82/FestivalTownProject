#pragma once
#include "Event.h"
#include "../Thread/TimerThread/TimerThread.h"



bool PushEventHeartBeat(Timer* pTimer, int sessionID) {
	EV_HEART_BEAT e;
	e.size = sizeof(EV_HEART_BEAT);
	e.type = eEventType::HEARTBEAT;
	e.sessionID = sessionID;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(HEARTBEATTIME);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_HEART_BEAT));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventObjectDrop(Timer* pTimer, int roomID, int time) {
	EV_OBJECT_DROP e;
	e.size = sizeof(EV_OBJECT_DROP);
	e.type = eEventType::OBJECTDROP;
	e.roomID = roomID;


	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(time);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_OBJECT_DROP));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventBombSpawn(Timer* pTimer, int roomID, int time)
{
	EV_BOMB_SPAWN e;
	e.size = sizeof(EV_BOMB_SPAWN);
	e.type = eEventType::BOMBSPAWN;
	e.roomID = roomID;


	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(time);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_BOMB_SPAWN));

	pTimer->PushEvent(header);

	return false;
}
