#pragma once
#include "Room.h"
#include "../PacketManager/PacketSender/PacketSender.h"
#include "../Server/Server.h"

Room::~Room()
{
	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mBombList[i];
	}
	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mWeaponList[i];
	}
	delete mMap;
}

void Room::Reset()
{
	mRoomID = 0; 
	mPlayerCnt = 0;
	mReadyCnt = 0;
	mHostID = INVALIDKEY;
	mRoomCode = 0;

	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mBombList[i];
	}
	for (int i = 0; i < MAXOBJECT; ++i) {
		delete mWeaponList[i];
	}
	delete mMap;

	std::fill(mPlayerList.begin(), mPlayerList.end(), INVALIDKEY);
	std::fill(mBombList.begin(), mBombList.end(), nullptr);
	std::fill(mWeaponList.begin(), mWeaponList.end(), nullptr);
	mMap = nullptr;

	mPlayerRecordList.clear();
	mTeams.clear();
	mState = eRoomState::RS_FREE;

	mAllPlayerReady = false;
	mIsRun = false;
}

void Room::Init(int id, GameMode gameMode, GameModeData& GameModeData)
{
	mRoomID = id;
	mGameMode = gameMode;
	mGameModeData = GameModeData;
	mHostID = INVALIDKEY;
	InitRoomCode();

	std::fill(mPlayerList.begin(), mPlayerList.end(), INVALIDKEY);

	// team game
	mTeams[(int)TeamCode::RED].Init(mGameModeData.Life_Count);
	mTeams[(int)TeamCode::BLUE].Init(mGameModeData.Life_Count);
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
	int sessionID = player->GetSessionID();

	for (int i = 0; i < MAXPLAYER; ++i) {
		int expected = INVALIDKEY;
		if (mPlayerList[i].compare_exchange_strong(expected, sessionID)) {
			player->SetInGameID(i);
			player->SetRoomID(mRoomID);

			// 외부에서 Session Lock 걸고 있음
			//player->GetSessionStateLock().lock();
			player->SetSessionState(eSessionState::ST_INGAME);
			//player->GetSessionStateLock().unlock();

			player->GetPlayerStateLock().lock();
			player->SetPlayerState(ePlayerState::PS_ALIVE);
			player->GetPlayerStateLock().unlock();
			mPlayerRecordList[i].Init();
			mPlayerRecordList[i].team = player->GetTeam();

			mTeams[player->GetTeam()].GetMembers().insert(i);

			return true;
		}
	}

	return false;
}

int Room::AddBomb(Bomb* object, Vector3f position, Vector3f direction)
{
	for (int i = 0; i < MAXOBJECT; ++i) {
		mBombListLock.lock();
		if (mBombList[i] == nullptr) {
			mBombList[i] = object;
			mBombListLock.unlock();
			object->SetID(i);
			object->SetPosition(position);
			object->SetDirection(direction);
			return i;
		}
		mBombListLock.unlock();
	}

	return INVALIDKEY;
}

int Room::AddWeapon(Weapon* object, Vector3f position, Vector3f direction)
{
	for (int i = 0; i < MAXOBJECT; ++i) {
		mWeaponListLock.lock();
		if (mWeaponList[i] == nullptr) {
			mWeaponList[i] = object;
			mWeaponListLock.unlock();
			object->SetID(i);
			object->SetPosition(position);
			object->SetDirection(direction);
			return i;
		}
		mWeaponListLock.unlock();
	}

	return INVALIDKEY;
}

bool Room::DeleteBomb(int id)
{
	mBombListLock.lock();
	if (mBombList.at(id) == nullptr) {
		mBombListLock.unlock();
		return false;
	}
	// 어떤 플레이어가 이 오브젝트를 가지고 있으면 해제시켜줘야함
	int OwnerID = mBombList.at(id)->GetOwenrID();
	if (OwnerID > INVALIDKEY) {
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[mPlayerList[OwnerID].load()]);

		player->GetSessionStateLock().lock();
		if (player->GetSessionState() == eSessionState::ST_INGAME) {
			player->SetBomb(nullptr);
		}
		player->GetSessionStateLock().unlock();
	}
	delete mBombList.at(id);
	mBombList.at(id) = nullptr;
	mBombListLock.unlock();
	return true;
}

bool Room::DeleteWeapon(int id)
{
	if (mWeaponList.at(id) == nullptr) {
		return false;
	}
	// 어떤 플레이어가 이 오브젝트를 가지고 있으면 해제시켜줘야함
	int OwnerID = mWeaponList.at(id)->GetOwenrID();
	if (OwnerID > INVALIDKEY) {
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[mPlayerList[OwnerID].load()]);

		player->GetSessionStateLock().lock();
		if (player->GetSessionState() == eSessionState::ST_INGAME) {
			player->SetWeapon(nullptr);
		}
		player->GetSessionStateLock().unlock();
	}
	delete mWeaponList.at(id);
	mWeaponList.at(id) = nullptr;
	return true;
}

void Room::GetAllObjects(std::vector<Object*>& objectList)
{
	mBombListLock.lock_shared();
	for (Bomb* bomb : mBombList) {
		if (bomb != nullptr) {
			objectList.push_back(bomb);
		}
	}
	mBombListLock.unlock_shared();

	mWeaponListLock.lock_shared();
	for (Weapon* weapon : mWeaponList) {
		if (weapon != nullptr) {
			objectList.push_back(weapon);
		}
	}
	mWeaponListLock.unlock_shared();
}

bool Room::SetIsRun(bool desired)
{
	bool expected = !desired;
	return mIsRun.compare_exchange_strong(expected, desired);
}

bool Room::SetAllPlayerReady(bool desired)
{
	bool expected = !desired;
	return mAllPlayerReady.compare_exchange_strong(expected, desired);
}

int Room::ChangeHost()
{
	int session_id = INVALIDKEY;
	for (const auto& id : mPlayerList) {
		session_id = id.load();
		if (session_id == INVALIDKEY) continue;
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[session_id]);
		mHostID = player->GetInGameID();
		break;
	}
	return session_id;
}
