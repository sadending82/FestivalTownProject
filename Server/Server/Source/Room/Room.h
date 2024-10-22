#pragma once
#include "Team/Team.h"
#include "../Object/Map/Map.h"
#include "../Object/Object.h"
#include "../Object/Bomb/Bomb.h"
#include "../Object/Weapon/Weapon.h"
#include "../TableManager/Tables/FITH.h"
#include "../Object/ObjectPool/ObjectPool.h"
#include <shared_mutex>

#define SESSION_ID std::atomic<int>

class Room
{
public:

	Room(class Server* server) :pServer(server), mState(eRoomState::RS_FREE) {};
	~Room();

	void Reset();
	void Init(int id, GameMode gameMode, GameModeData& GameModeData);

	void InitMap(Map* map) { mMap = new Map(*map); }
	void InitRoomCode();

	bool AddPlayer(Player* player);

	int  AddBomb(Vector3f position, Vector3f direction = Vector3f(0, 0, 0));
	int  AddWeapon(eWeaponType weaponType, WeaponStat& weaponStat, Vector3f position, Vector3f direction = Vector3f(0, 0, 0));

	bool DeleteBomb(int id);
	bool DeleteWeapon(int id);

	std::array<SESSION_ID, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::unordered_map<int, Team>& GetTeams() { return mTeams; }
	std::unordered_map<int, sPlayerGameRecord>& GetPlayerRecordList() { return mPlayerRecordList; }

	ObjectPool& GetObjectPool() { return mObjectPool; }

	Weapon* GetWeapon(int id);
	Bomb* GetBomb(int id);

	int GetPlayerCnt() { return mPlayerCnt.load(); }
	int GetReadyCnt() { return mReadyCnt.load(); }
	GameMode GetGameMode() { return mGameMode; }
	Map* GetMap() { return mMap; }
	TIMEPOINT GetStartTime() { return mStartTime; }
	eRoomState GetState() { return mState; }
	std::shared_mutex& GetStateLock() { return mStateLock; }

	int GetHostID() { return mHostID; }
	long long GetRoomCode() { return mRoomCode; }
	bool GetAllPlayerReady() { return mAllPlayerReady.load(); }
	bool GetIsRun() { return mIsRun.load(); }
	GameModeData& GetGameModeData() { return mGameModeData; }
	

	void AddPlayerCnt() { mPlayerCnt.fetch_add(1); }
	void SubPlayerCnt() { mPlayerCnt.fetch_sub(1); }
	void SetPlayerCnt(int count) { mPlayerCnt.store(count); }
	void AddReadyCnt() { mReadyCnt.fetch_add(1); }
	void SetGameMode(GameMode GameMode) { mGameMode = GameMode; }
	void SetStartTime(TIMEPOINT time) { mStartTime = time; }
	void SetState(eRoomState state) { mState = state; }
	void SetHost(int ingameid) { mHostID = ingameid; }
	bool SetIsRun(bool desired);
	bool SetAllPlayerReady(bool desired);

	int ChangeHost();

private:
	eRoomState mState;
	GameMode mGameMode;
	Map* mMap = nullptr;

	class Server* pServer = nullptr;

	std::shared_mutex mStateLock;

	std::array<SESSION_ID, MAXPLAYER> mPlayerList;

	ObjectPool mObjectPool;

	std::unordered_map<int, sPlayerGameRecord> mPlayerRecordList;
	std::unordered_map<int, Team> mTeams;

	int mRoomID; // room array index
	std::atomic<int> mPlayerCnt = 0;
	std::atomic<int> mReadyCnt = 0;
	int mHostID = INVALIDKEY;
	TIMEPOINT mStartTime;
	long long mRoomCode = 0; // Room 고유 식별 번호

	std::atomic<bool> mAllPlayerReady = false;
	std::atomic<bool> mIsRun = false;

	GameModeData mGameModeData;
};

