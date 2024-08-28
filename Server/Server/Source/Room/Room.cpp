#include "Room.h"

Room::~Room()
{
	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mObjectList[i];
	}
}

void Room::Reset()
{
	mRoomID = 0; 
	mPlayerCnt = 0;
	mPlayerLimit = 0;
	mHostID = INVALIDKEY;
	mRoomCode = 0;

	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mObjectList[i];
	}
	std::fill(mPlayerList.begin(), mPlayerList.end(), nullptr);
	std::fill(mObjectList.begin(), mObjectList.end(), nullptr);
	mTeams.clear();
	mStateLock.lock();
	mState = eRoomState::RS_FREE;
	mStateLock.unlock();
}

void Room::Init(int id, int teamLifeCount, int playerLimit)
{
	mRoomID = id;
	mPlayerCnt = 0;
	mPlayerLimit = playerLimit;
	mHostID = INVALIDKEY;

	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mObjectList[i];
	}
	std::fill(mPlayerList.begin(), mPlayerList.end(), nullptr);
	std::fill(mObjectList.begin(), mObjectList.end(), nullptr);
	mTeams.clear();

	// team game
	mTeams[TeamCode::RED].Init(teamLifeCount);
	mTeams[TeamCode::BLUE].Init(teamLifeCount);

	InitRoomCode();
}


void Room::InitRoomCode()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	int numDigits = log10(mRoomID) + 1;
	mRoomCode = millis * pow(10, numDigits) + mRoomID;
}

bool Room::AddPlayer(Player* player)
{
	for (int i = 0; i < MAXPLAYER; ++i) {
		mPlayerListLock.lock();
		if (mPlayerList[i] == nullptr) {
			mPlayerList[i] = player;

			mPlayerListLock.unlock();
			player->SetInGameID(i);
			player->SetRoomID(mRoomID);
			player->SetState(eSessionState::ST_INGAME);

			player->GetPlayerStateLock().lock();
			player->SetPlayerState(ePlayerState::PS_RUNNING);
			player->GetPlayerStateLock().unlock();
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

int Room::AddBomb(Bomb* object, Vector3f position, Vector3f direction)
{
	for (int i = 0; i < MAXOBJECT; ++i) {
		mObjectListLock.lock();
		if (mObjectList[i] == nullptr) {
			mObjectList[i] = object;
			mObjectListLock.unlock();
			object->SetID(i);
			object->SetPosition(position);
			object->SetDirection(direction);
			return i;
		}
		mObjectListLock.unlock();
	}

	return INVALIDKEY;
}

bool Room::DeleteObject(int id)
{
	mObjectListLock.lock();
	delete mObjectList[id];
	mObjectList[id] = nullptr;
	mObjectListLock.unlock();
	return true;
}

bool Room::SetIsRun(bool desired)
{
	bool expected = !desired;
	return mIsRun.compare_exchange_strong(expected, desired);
}

int Room::ChangeHost()
{
	int sessionid = INVALIDKEY;
	mPlayerListLock.lock();
	for (Player* player : mPlayerList) {
		if (player == nullptr) {
			continue;
		}
		mHostID = player->GetInGameID();
		sessionid = player->GetSessionID();
		break;
	}
	mPlayerListLock.unlock();
	return sessionid;
}
