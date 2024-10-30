#pragma once
#include "Room.h"
#include "../PacketManager/PacketSender/PacketSender.h"
#include "../Server/Server.h"

Room::~Room()
{
	delete mMap;
}

void Room::Reset()
{
	mRoomID = 0; 
	mPlayerCnt = 0;
	mReadyCnt = 0;
	mHostID = INVALIDKEY;
	mRoomCode = 0;

	mObjectPool.AllObjectDeactive();

	std::fill(mPlayerList.begin(), mPlayerList.end(), INVALIDKEY);

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

	mObjectPool.AllObjectDeactive();
	std::fill(mPlayerList.begin(), mPlayerList.end(), INVALIDKEY);

	// team game
	mTeams[(int)TeamCode::RED].Init(mGameModeData.Life_Count);
	mTeams[(int)TeamCode::BLUE].Init(mGameModeData.Life_Count);
}


void Room::InitMap(Map* map)
{
	delete mMap;
	mMap = new Map(*map);
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
			player->SetSessionState(eSessionState::ST_GAMELOADING);
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

int Room::AddBomb(Vector3f position, Vector3f direction)
{
	Bomb* bomb = dynamic_cast<Bomb*>(mObjectPool.GetFreeObject(eObjectType::BOMB));

	if (bomb == nullptr) { return INVALIDKEY; }

	bomb->BombInit();
	bomb->SetPosition(position);
	bomb->SetDirection(direction);

	return bomb->GetID();
}

int Room::AddWeapon(eWeaponType weaponType, WeaponStat& weaponStat, Vector3f position, Vector3f direction)
{
	Weapon* weapon = dynamic_cast<Weapon*>(mObjectPool.GetFreeObject(eObjectType::WEAPON));

	if (weapon == nullptr) { return INVALIDKEY; }

	weapon->WeaponInit(weaponType, weaponStat);
	weapon->SetPosition(position);
	weapon->SetDirection(direction);

	return weapon->GetID();
}

bool Room::DeleteBomb(int id)
{
	Bomb* bomb = GetBomb(id);
	if (bomb == nullptr) {
		return false;
	}
	// 어떤 플레이어가 이 오브젝트를 가지고 있으면 해제시켜줘야함
	int OwnerID = bomb->GetOwenrID();
	if (OwnerID > INVALIDKEY) {
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[mPlayerList[OwnerID].load()]);

		player->GetSessionStateLock().lock();
		if (player->GetSessionState() == eSessionState::ST_INGAME) {
			player->SetBomb(nullptr);
		}
		player->GetSessionStateLock().unlock();
	}
	bomb->SetIsActive(false);
	return true;
}

bool Room::DeleteWeapon(int id)
{
	Weapon* weapon = GetWeapon(id);
	if (weapon == nullptr) {
		return false;
	}
	// 어떤 플레이어가 이 오브젝트를 가지고 있으면 해제시켜줘야함
	int OwnerID = weapon->GetOwenrID();
	if (OwnerID > INVALIDKEY) {
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[mPlayerList[OwnerID].load()]);

		player->GetSessionStateLock().lock();
		if (player->GetSessionState() == eSessionState::ST_INGAME) {
			player->SetWeapon(nullptr);
		}
		player->GetSessionStateLock().unlock();
	}
	weapon->SetIsActive(false);
	return true;
}

Weapon* Room::GetWeapon(int id)
{
	return dynamic_cast<Weapon*>(mObjectPool.GetObjectByID(id));
}

Bomb* Room::GetBomb(int id)
{
	return dynamic_cast<Bomb*>(mObjectPool.GetObjectByID(id));
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

void Room::ChangeAllPlayerInGame()
{
	for (const auto& sID : mPlayerList) {
		int id = sID.load();
		if (id == INVALIDKEY) continue;
		Player* p = dynamic_cast<Player*>(pServer->GetSessions()[id]);
		p->GetSessionStateLock().lock();
		p->SetSessionState(eSessionState::ST_INGAME);
		p->GetSessionStateLock().unlock();
	}
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
