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
	void Init(int id, GameMode gameMode, GameModeOutData& GameModeOutData, GameModeData& GameModeData);

	void InitMap(Map* map, int mapTheme);
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
	GameMode GetGameMode() { return mGameMode; }
	Map* GetMap() { return mMap; }
	TIMEPOINT GetStartTime() { return mStartTime; }
	eRoomState GetState() { return mState; }
	std::shared_mutex& GetStateLock() { return mStateLock; }

	int GetHostID() { return mHostID; }
	long long GetRoomCode() { return mRoomCode; }
	bool GetAllPlayerReady() { return mAllPlayerReady.load(); }
	bool GetIsRun() { return mIsRun.load(); }
	GameModeOutData& GetGameModeOutData() { return mGameModeOutData; }
	GameModeData& GetGameModeData() { return mGameModeData; }
	int GetmapIndex() { return mMap->GetMapIndex(); }
	int GetMapTheme() { return mMapTheme; }
	bool GetIsTestRoom() { return mIsTestRoom; }
	

	void AddPlayerCnt() { mPlayerCnt.fetch_add(1); }
	void SubPlayerCnt() { mPlayerCnt.fetch_sub(1); }
	void SetPlayerCnt(int count) { mPlayerCnt.store(count); }
	void SetIsPlayerReady(int playerID, bool value);
	void SetGameMode(GameMode GameMode) { mGameMode = GameMode; }
	void SetStartTime(TIMEPOINT time) { mStartTime = time; }
	void SetState(eRoomState state) { mState = state; }
	void SetHost(int ingameid) { mHostID = ingameid; }
	bool SetIsRun(bool desired);
	bool SetAllPlayerReady(bool desired);
	void SetIsTestRoom(bool value) { mIsTestRoom = value; }

	void ChangeAllPlayerInGame();

	int ChangeHost();

	bool CheckAllPlayerReady();

private:
	eRoomState mState;
	GameMode mGameMode;
	Map* mMap = nullptr;
	int mMapTheme = 0;

	class Server* pServer = nullptr;

	std::shared_mutex mStateLock;

	std::mutex mIsReadyListLock;

	std::array<SESSION_ID, MAXPLAYER> mPlayerList;
	std::array<std::atomic<bool>, MAXPLAYER> mIsReadyList;

	ObjectPool mObjectPool;

	std::unordered_map<int, sPlayerGameRecord> mPlayerRecordList;
	std::unordered_map<int, Team> mTeams;

	int mRoomID; // room array index
	std::atomic<int> mPlayerCnt = 0;
	int mHostID = INVALIDKEY;
	TIMEPOINT mStartTime;
	long long mRoomCode = 0; // Room 고유 식별 번호

	std::atomic<bool> mAllPlayerReady = false;
	std::atomic<bool> mIsRun = false;
	bool	mIsTestRoom = false;

	GameModeOutData mGameModeOutData;
	GameModeData mGameModeData;
};

