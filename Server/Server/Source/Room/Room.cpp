#include "Room.h"

Room::Room()
{
}

Room::~Room()
{
}

void Room::Init(int id)
{
	mRoomID = id;
	std::fill(mPlayerList.begin(), mPlayerList.end(), nullptr);
}

void Room::addPlayer(Player* player)
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
			return;
		}
		mPlayerListLock.unlock();
	}
}