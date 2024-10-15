#pragma once
#include "Team/Team.h"
#include "../Object/Map/Map.h"
#include "../Object/Object.h"
#include "../Object/Bomb/Bomb.h"
#include "../Object/Weapon/Weapon.h"
#include "../TableManager/Tables/FITH.h"
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

	int  AddBomb(Bomb* object, Vector3f position, Vector3f direction = Vector3f(0, 0, 0));
	int  AddWeapon(Weapon* object, Vector3f position, Vector3f direction = Vector3f(0, 0, 0));

	bool DeleteBomb(int id);
	bool DeleteWeapon(int id);

	std::array<SESSION_ID, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::unordered_map<int, Team>& GetTeams() { return mTeams; }
	std::unordered_map<int, sPlayerGameRecord>& GetPlayerRecordList() { return mPlayerRecordList; }
	std::array<Bomb*, MAXOBJECT>& GetBombList() { return mBombList; }
	std::array<Weapon*, MAXOBJECT>& GetWeaponList() { return mWeaponList; }
	void GetAllObjects(std::vector<Object*>& objectList);
	int GetPlayerCnt() { return mPlayerCnt; }
	int GetReadyCnt() { return mReadyCnt; }
	GameMode GetGameMode() { return mGameMode; }
	Map* GetMap() { return mMap; }
	TIMEPOINT GetStartTime() { return mStartTime; }
	eRoomState GetState() { return mState; }
	std::shared_mutex& GetStateLock() { return mStateLock; }
	std::shared_mutex& GetBombListLock() { return mBombListLock; }
	std::shared_mutex& GetWeaponListLock() { return mWeaponListLock; }
	int GetHostID() { return mHostID; }
	long long GetRoomCode() { return mRoomCode; }
	bool GetAllPlayerReady() { return mAllPlayerReady.load(); }
	bool GetIsRun() { return mIsRun.load(); }
	GameModeData& GetGameModeData() { return mGameModeData; }
	

	void AddPlayerCnt() { mPlayerCnt++; }
	void SubPlayerCnt() { mPlayerCnt--; }
	void AddReadyCnt() { mReadyCnt++; }
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
	std::shared_mutex mBombListLock;
	std::shared_mutex mWeaponListLock;
	std::array<SESSION_ID, MAXPLAYER> mPlayerList;
	std::array<Bomb*, MAXOBJECT> mBombList = { nullptr };
	std::array<Weapon*, MAXOBJECT> mWeaponList = { nullptr };
	std::unordered_map<int, sPlayerGameRecord> mPlayerRecordList;
	std::unordered_map<int, Team> mTeams;

	int mRoomID; // room array index
	int mPlayerCnt = 0;
	int mReadyCnt = 0;
	int mHostID = INVALIDKEY;
	TIMEPOINT mStartTime;
	long long mRoomCode = 0; // Room 고유 식별 번호

	std::atomic<bool> mAllPlayerReady = false;
	std::atomic<bool> mIsRun = false;

	GameModeData mGameModeData;
};

