#pragma once
#include "Team/Team.h"
#include "../Object/Map/Map.h"
#include "../Object/Object.h"
#include "../Object/Bomb/Bomb.h"

class Room
{
public:

	Room() :mState(eRoomState::RS_FREE) {};
	~Room();

	void Reset();
	void Init(int id, int teamLifeCount, int playerLimit = MAXPLAYER);

	void InitMap(Map& map) { mMap = map; }
	void InitRoomCode();

	bool AddPlayer(Player* player);
	bool DeletePlayer(int playerID);

	int  AddBomb(Bomb* object, Vector3f position, Vector3f direction = Vector3f(0, 0, 0));
	bool DeleteObject(int id);

	std::array<Player*, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::unordered_map<int, Team>& GetTeams() { return mTeams; }
	std::array<Object*, MAXOBJECT>& GetObjects() { return mObjectList; }
	int GetPlayerCnt() { return mPlayerCnt; }
	int GetPlayerLimit() { return mPlayerLimit; }
	GameCode GetGameMode() { return mGameMode; }
	Map& GetMap() { return mMap; }
	TIMEPOINT GetStartTime() { return mStartTime; }
	eRoomState GetState() { return mState; }
	std::mutex& GetStateLock() { return mStateLock; }
	int GetHostID() { return mHostID; }
	long long GetRoomCode() { return mRoomCode; }
	

	void AddPlayerCnt() { mPlayerCnt++; }
	void SetPlayerLimit(int num) { mPlayerLimit = num; }
	void SetGameMode(GameCode gamecode) { mGameMode = gamecode; }
	void SetStartTime(TIMEPOINT time) { mStartTime = time; }
	void SetState(eRoomState state) { mState = state; }
	void SetHost(int ingameid) { mHostID = ingameid; }

private:
	eRoomState mState;
	GameCode mGameMode;
	Map mMap;

	std::mutex mStateLock;
	std::mutex mPlayerListLock;
	std::mutex mObjectListLock;
	std::array<Player*, MAXPLAYER> mPlayerList;
	std::array<Object*, MAXOBJECT> mObjectList;

	std::mutex mPlayerSessionIDsLock;
	std::unordered_map<int, Team> mTeams;

	int mRoomID; // room array index
	int mPlayerCnt = 0;
	int mPlayerLimit = 6;
	int mHostID = INVALIDKEY;
	TIMEPOINT mStartTime;
	long long mRoomCode = 0; // 이벤트 처리용 고유 식별 번호
};

