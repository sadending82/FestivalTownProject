#include "Room.h"

Room::~Room()
{
}

void Room::Init(int id, int teamLifeCount, int playerLimit)
{
	mRoomID = id;
	mPlayerLimit = playerLimit;
	std::fill(mPlayerList.begin(), mPlayerList.end(), nullptr);
	std::fill(mObjectList.begin(), mObjectList.end(), nullptr);

	// team game
	mTeams[TeamCode::RED].Init(teamLifeCount);
	mTeams[TeamCode::BLUE].Init(teamLifeCount);
}


bool Room::AddPlayer(Player* player)
{
	for (int i = 0; i < MAXPLAYER; ++i) {
		mPlayerListLock.lock();
		if (mPlayerList[i] == nullptr) {
			mPlayerList[i] = player;

			mPlayerListLock.unlock();
			player->SetInGameID(i);
			player->SetState(eSessionState::ST_INGAME);
			player->SetRoomID(mRoomID);
			mPlayerSessionIDs[i] = player->GetSessionID();
			return true;
		}
		mPlayerListLock.unlock();
	}

	return false;
}

bool Room::DeletePlayer(int playerID)
{
	if (mPlayerList[playerID] == nullptr) {
		return false;
	}
	mPlayerListLock.lock();
	mPlayerList[playerID] = nullptr;
	mPlayerListLock.unlock();
	if (mPlayerCnt > 0) {
		mPlayerCnt--;
	}
	return true;
}

bool Room::AddObject(Object* object, Vector3f position, Vector3f direction)
{
	for (int i = 0; i < MAXPLAYER; ++i) {
		if (mObjectList[i] == nullptr) {
			mObjectList[i] = object;

			object->SetID(i);
			object->SetPosition(position);
			object->SetDirection(direction);
			return true;
		}
	}

	return false;
}

bool Room::DeleteObject(int id)
{
	if (mObjectList[id] == nullptr) {
		return false;
	}
	mObjectList[id] = nullptr;
	return true;
}
