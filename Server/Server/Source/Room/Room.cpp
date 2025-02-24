#pragma once
#include "Room.h"
#include "../PacketManager/PacketSender/PacketSender.h"
#include "../Server/Server.h"

Room::~Room()
{
	
}

void Room::Reset()
{
	mRoomID = 0; 
	mPlayerCnt = 0;
	mHostID = INVALIDKEY;
	mRoomCode = 0;

	mObjectPool.AllObjectDeactive();

	std::fill(mPlayerList.begin(), mPlayerList.end(), INVALIDKEY);
	std::fill(mIsReadyList.begin(), mIsReadyList.end(), false);

	mPlayerRecordList.clear();
	mTeams.clear();
	mState = eRoomState::RS_FREE;

	mAllPlayerReady = false;
	mIsRun = false;
}

void Room::Init(int id, GameMode gameMode, GameModeOutData& GameModeOutData, GameModeData& GameModeData)
{
	mRoomID = id;
	mGameMode = gameMode;
	mGameModeOutData = GameModeOutData;
	mGameModeData = GameModeData;
	mHostID = INVALIDKEY;
	InitRoomCode();

	mObjectPool.AllObjectDeactive();
	std::fill(mPlayerList.begin(), mPlayerList.end(), INVALIDKEY);


	int teamCount = mGameModeOutData.Team_Count;

	for (int i = 0; i < teamCount; ++i) {
		mTeams[i].Init(mGameModeData.Life_Count);
	}
}


void Room::InitMap(Map& map, int mapTheme)
{
	mMap = map;
	mMapTheme = mapTheme;
}

void Room::InitRoomCode()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	int numDigits = log10(millis) + 1;
	mRoomCode = (mRoomID + 1) * pow(10, numDigits) + millis;
}

bool Room::AddPlayer(Player* player)
{
	int sessionID = player->GetSessionID();

	for (int i = 0; i < MAXPLAYER; ++i) {
		int expected = INVALIDKEY;
		if (mPlayerList[i].compare_exchange_strong(expected, sessionID)) {
			player->SetInGameID(i);
			player->SetroomID(mRoomID);
			
			player->SetSessionState(eSessionState::ST_GAMELOADING);

			player->SetPlayerState(ePlayerState::PS_ALIVE);
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

		if (mPlayerList[OwnerID].load() == INVALIDKEY) {
			return false;
		}
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[mPlayerList[OwnerID].load()]);

		if (player->GetSessionState() == eSessionState::ST_INGAME) {
			player->SetBomb(nullptr);
		}
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

		if (mPlayerList[OwnerID].load() == INVALIDKEY) {
			return false;
		}

		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[mPlayerList[OwnerID].load()]);

		if (player->GetSessionState() == eSessionState::ST_INGAME) {
			player->SetWeapon(nullptr);
		}
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

void Room::SetIsPlayerReady(int playerID, bool value)
{
	mIsReadyListLock.lock();
	mIsReadyList[playerID].store(value);
	mIsReadyListLock.unlock();
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
		p->ChangeSessionState(eSessionState::ST_GAMELOADING, eSessionState::ST_INGAME);
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

bool Room::CheckAllPlayerReady()
{
	mIsReadyListLock.lock();
	for (int i = 0; i < MAXPLAYER; ++i) {
		if (mPlayerList[i].load() == INVALIDKEY) {
			continue;
		}
		bool isReady = mIsReadyList[i].load();
		if (isReady == false) {
			mIsReadyListLock.unlock();
			return false;
		}
	}
	mIsReadyListLock.unlock();
	return true;
}
