#pragma once
#include "Team/Team.h"
#include "../Object/Map/Map.h"

class Room
{
public:

	Room() :mState(eRoomState::RS_FREE) {};
	~Room();

	void Init(int id, int teamLifeCount, int playerLimit = MAXPLAYER);

	void InitMap(Map& map) { mMap = map; }

	bool addPlayer(Player* player);
	bool DeletePlayer(int playerID);

	std::array<Player*, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::array<int, MAXPLAYER>& GetPlayerIDs() { return mPlayerSessionIDs; }
	std::unordered_map<int, Team>& GetTeams() { return mTeams; }
	int GetPlayerCnt() { return mPlayerCnt; }
	int GetPlayerLimit() { return mPlayerLimit; }
	GameCode GetGameMode() { return mGameMode; }
	Map& GetMap() { return mMap; }
	TIMEPOINT GetStartTime() { return mStartTime; }
	eRoomState GetState() { return mState; }
	

	void AddPlayerCnt() { mPlayerCnt++; }
	void SetPlayerLimit(int num) { mPlayerLimit = num; }
	void SetGameMode(GameCode gamecode) { mGameMode = gamecode; }
	void SetStartTime(TIMEPOINT time) { mStartTime = time; }
	void SetState(eRoomState state) { mState = state; }

private:
	eRoomState mState;
	GameCode mGameMode;
	Map mMap;

	std::mutex mPlayerListLock;
	std::array<Player*, MAXPLAYER> mPlayerList;

	std::mutex mPlayerSessionIDsLock;
	std::array<int, MAXPLAYER> mPlayerSessionIDs;
	std::unordered_map<int, Team> mTeams;

	int mRoomID;
	int mPlayerCnt = 0;
	int mPlayerLimit = 6;
	TIMEPOINT mStartTime;
};

