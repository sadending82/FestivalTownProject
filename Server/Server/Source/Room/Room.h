#pragma once
#include "Team/Team.h"
#include "../Object/Map/Map.h"
#include "../Object/Object.h"
#include "../Object/Bomb/Bomb.h"
#include "../Object/Weapon/Weapon.h"

class Room
{
public:

	Room() :mState(eRoomState::RS_FREE) {};
	~Room();

	void Reset();
	void Init(int id, int teamLifeCount, int playerLimit = MAXPLAYER);

	void InitMap(Map* map) { mMap = new Map(*map); }
	void InitRoomCode();

	bool AddPlayer(Player* player);
	bool DeletePlayer(int playerID);

	int  AddBomb(Bomb* object, Vector3f position, Vector3f direction = Vector3f(0, 0, 0));
	int  AddWeapon(Weapon* object, Vector3f position, Vector3f direction = Vector3f(0, 0, 0));

	bool DeleteBomb(int id);
	bool DeleteWeapon(int id);

	std::array<Player*, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::unordered_map<int, Team>& GetTeams() { return mTeams; }
	std::unordered_map<int, sPlayerGameRecord>& GetPlayerRecordList() { return mPlayerRecordList; }
	std::array<Bomb*, MAXOBJECT>& GetBombList() { return mBombList; }
	std::array<Weapon*, MAXOBJECT>& GetWeaponList() { return mWeaponList; }
	void GetAllObjects(std::vector<Object*>& objectList);
	int GetPlayerCnt() { return mPlayerCnt; }
	int GetReadyCnt() { return mPlayerCnt; }
	int GetPlayerLimit() { return mPlayerLimit; }
	GameMode GetGameMode() { return mGameMode; }
	Map* GetMap() { return mMap; }
	TIMEPOINT GetStartTime() { return mStartTime; }
	eRoomState GetState() { return mState; }
	std::mutex& GetStateLock() { return mStateLock; }
	std::shared_mutex& GetPlayerListLock() { return mPlayerListLock; }
	std::shared_mutex& GetBombListLock() { return mBombListLock; }
	std::shared_mutex& GetWeaponListLock() { return mWeaponListLock; }
	int GetHostID() { return mHostID; }
	long long GetRoomCode() { return mRoomCode; }
	bool GetAllPlayerReady() { return mAllPlayerReady.load(); }
	bool GetIsRun() { return mIsRun.load(); }
	

	void AddPlayerCnt() { mPlayerCnt++; }
	void AddReadyCnt() { mPlayerCnt++; }
	void SetPlayerLimit(int num) { mPlayerLimit = num; }
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

	std::mutex mStateLock;
	std::shared_mutex mPlayerListLock;
	std::shared_mutex mBombListLock;
	std::shared_mutex mWeaponListLock;
	std::array<Player*, MAXPLAYER> mPlayerList = { nullptr };
	std::array<Bomb*, MAXOBJECT> mBombList = { nullptr };
	std::array<Weapon*, MAXOBJECT> mWeaponList = { nullptr };
	std::unordered_map<int, sPlayerGameRecord> mPlayerRecordList;
	std::unordered_map<int, Team> mTeams;

	int mRoomID; // room array index
	int mPlayerCnt = 0;
	int mReadyCnt = 0;
	int mPlayerLimit = 6;
	int mHostID = INVALIDKEY;
	TIMEPOINT mStartTime;
	long long mRoomCode = 0; // Room 고유 식별 번호

	std::atomic<bool> mAllPlayerReady = false;
	std::atomic<bool> mIsRun = false;
};

