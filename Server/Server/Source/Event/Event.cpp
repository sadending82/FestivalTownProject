#pragma once
#include "Event.h"
#include "../Thread/TimerThread/TimerThread.h"

bool PushEventHeartBeat(Timer* pTimer, int sessionID) {
	EV_HEART_BEAT e;
	e.size = sizeof(EV_HEART_BEAT);
	e.type = eEventType::HEARTBEAT;
	e.sessionID = sessionID;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(HEARTBEATINTERVAL);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_HEART_BEAT));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventGameMatching(Timer* pTimer, int requesterID, eMatchingType matchingType, long long requestTime, int intervalSecond)
{
	EV_GAME_MATCHING e;
	e.size = sizeof(EV_GAME_MATCHING);
	e.type = eEventType::GAMEMATCHING;
	e.requesterID = requesterID;
	e.matchingType = matchingType;
	e.requestTime = requestTime;


	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_GAME_MATCHING));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventGameStart(Timer* pTimer, int roomID, long long roomCode)
{
	EV_GAME_START e;
	e.size = sizeof(EV_GAME_START);
	e.type = eEventType::GAMESTART;
	e.roomID = roomID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(GAMESTARTINTERVAL);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_GAME_START));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventBlockDrop(Timer* pTimer, int roomID, long long roomCode, int blockType, int intervalSecond) {
	EV_OBJECT_DROP e;
	e.size = sizeof(EV_OBJECT_DROP);
	e.type = eEventType::BLOCKDROP;
	e.roomID = roomID;
	e.roomCode = roomCode;
	e.objectType = blockType;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_OBJECT_DROP));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventBombSpawn(Timer* pTimer, int roomID, long long roomCode, int intervalSecond)
{
	EV_BOMB_SPAWN e;
	e.size = sizeof(EV_BOMB_SPAWN);
	e.type = eEventType::BOMBSPAWN;
	e.roomID = roomID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_BOMB_SPAWN));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventWeaponSpawn(Timer* pTimer, int roomID, long long roomCode, int weaponType, int intervalSecond)
{
	EV_WEAPON_SPAWN e;
	e.size = sizeof(EV_WEAPON_SPAWN);
	e.type = eEventType::WEAPONSPAWN;
	e.roomID = roomID;
	e.roomCode = roomCode;
	e.weaponType = weaponType;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_WEAPON_SPAWN));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventRemainTimeSync(Timer* pTimer, int roomID, long long roomCode)
{
	EV_TIME_SYNC e;
	e.size = sizeof(EV_TIME_SYNC);
	e.type = eEventType::TIMESYNC;
	e.roomID = roomID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(REMAINTIMESYNCINTERVAL);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_TIME_SYNC));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventBombExplosion(Timer* pTimer, int roomID, int bombID, long long roomCode, int intervalSecond)
{
	EV_BOMB_EXPLOSION e;
	e.size = sizeof(EV_BOMB_EXPLOSION);
	e.type = eEventType::BOMBEXPLOSION;
	e.roomID = roomID;
	e.bombID = bombID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_BOMB_EXPLOSION));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventTimeOverCheck(Timer* pTimer, int roomID, long long roomCode)
{
	EV_TIMEOVER_CHECK e;
	e.size = sizeof(EV_TIMEOVER_CHECK);
	e.type = eEventType::TIMEOVERCHECK;
	e.roomID = roomID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOVERCHECKINTERVAL);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_TIMEOVER_CHECK));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventPlayerRespawn(Timer* pTimer, int playerID, int roomID, long long roomCode, int intervalSecond)
{
	EV_PLAYER_RESPAWN e;
	e.size = sizeof(EV_PLAYER_RESPAWN);
	e.type = eEventType::PLAYERRESPAWN;
	e.playerID = playerID;
	e.roomID = roomID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_PLAYER_RESPAWN));

	pTimer->PushEvent(header);

	return true;
}

bool PushEventGroggyRecovery(Timer* pTimer, int playerID, int roomID, long long roomCode, int intervalSecond)
{
	EV_GROGGY_RECOVERY e;
	e.size = sizeof(EV_GROGGY_RECOVERY);
	e.type = eEventType::GROGGYRECOVERY;
	e.playerID = playerID;
	e.roomID = roomID;
	e.roomCode = roomCode;

	EVENT_HEADER header;
	header.start_time = std::chrono::system_clock::now() + std::chrono::seconds(intervalSecond);
	memcpy(header.message, reinterpret_cast<char*>(&e), sizeof(EV_GROGGY_RECOVERY));

	pTimer->PushEvent(header);

	return true;
}
