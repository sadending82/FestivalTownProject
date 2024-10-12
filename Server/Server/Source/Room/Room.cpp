#pragma once
#include "Room.h"
#include "../PacketManager/PacketSender/PacketSender.h"

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

	std::fill(mPlayerList.begin(), mPlayerList.end(), nullptr);
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
	for (int i = 0; i < MAXPLAYER; ++i) {
		mPlayerListLock.lock();
		if (mPlayerList[i] == nullptr) {
			mPlayerList[i] = player;

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

			mPlayerListLock.unlock();

			return true;
		}
		mPlayerListLock.unlock();
	}

	return false;
}

bool Room::DeletePlayer(int playerID, PacketSender* packetSender)
{
	if (mState != eRoomState::RS_INGAME) {
		return false;
	}

	Player* player = mPlayerList[playerID];

	if (player == nullptr) {
		return false;
	}
	Vector3f pos = player->GetPosition();
	Vector3f dir = player->GetDirection();

	// 들고있는 무기 해제
	player->GetWeaponLock().lock();
	Weapon* weapon = player->GetWeapon();
	if (weapon != nullptr) {
		if (weapon->SetIsGrabbed(false) == true) {
			int weaponID = weapon->GetID();
			weapon->SetOwenrID(INVALIDKEY);
			weapon->SetPosition(player->GetPosition());
			packetSender->SendWeaponDropPacket(pos, mRoomID, weaponID);
		}
	}
	player->GetWeaponLock().unlock();

	// 들고있는 폭탄 폭발
	player->GetBombLock().lock();
	Bomb* bomb = player->GetBomb();
	if (bomb != nullptr) {
		if (bomb->SetIsGrabbed(false) == true) {
			int bombID = bomb->GetID();
			bomb = nullptr;
			packetSender->SendBombExplosionPacket(mRoomID, bombID);
			DeleteBomb(bombID);
		}
	}
	player->GetBombLock().unlock();

	// 잡은 플레이어 놓기
	if (player->GetAttachedPlayerID() != INVALIDKEY && player->GetIsGrabbed() == false) {
		int targetID = player->GetAttachedPlayerID();
		Player* target = mPlayerList[player->GetAttachedPlayerID()];

		if (target->SetIsGrabbed(false) == true) {
			player->SetAttachedPlayerID(INVALIDKEY);
			target->SetAttachedPlayerID(INVALIDKEY);
			packetSender->SendPlayerThrowOtherPlayerPacket(mRoomID, playerID, pos, dir, targetID, target->GetPosition(), target->GetDirection());
		}
	}

	mTeams[mPlayerList[playerID]->GetTeam()].GetMembers().erase(playerID);
	mPlayerList[playerID] = nullptr;
	if (mPlayerCnt > 0) {
		mPlayerCnt--;
	}
	return true;
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
		mPlayerListLock.lock_shared();
		Player* player = mPlayerList.at(OwnerID);
		if (player != nullptr) {
			mPlayerList.at(OwnerID)->SetBomb(nullptr);
		}
		mPlayerListLock.unlock_shared();
	}
	delete mBombList.at(id);
	mBombList.at(id) = nullptr;
	mBombListLock.unlock();
	return true;
}

bool Room::DeleteWeapon(int id)
{
	mWeaponListLock.lock();
	if (mWeaponList.at(id) == nullptr) {
		mWeaponListLock.unlock();
		return false;
	}
	// 어떤 플레이어가 이 오브젝트를 가지고 있으면 해제시켜줘야함
	int OwnerID = mWeaponList[id]->GetOwenrID();
	if (OwnerID > INVALIDKEY) {
		mPlayerListLock.lock_shared();
		Player* player = mPlayerList.at(OwnerID);
		if (player != nullptr) {
			mPlayerList.at(OwnerID)->SetWeapon(nullptr);
		}
		mPlayerListLock.unlock_shared();
	}
	delete mWeaponList.at(id);
	mWeaponList.at(id) = nullptr;
	mWeaponListLock.unlock();
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
