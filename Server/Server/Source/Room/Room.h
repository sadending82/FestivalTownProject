#pragma once
#include "../utility.h"
#include "../Object/Player.h"
#include "../Object/Map/Map.h"

class Room
{
public:

	Room();
	~Room();

	void Init(int id, int playerLimit = MAXPLAYER);

	void InitMap(Map& map) { mMap = map; }

	bool addPlayer(Player* player);
	bool DeletePlayer(int playerID);

	std::array<Player*, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::array<int, MAXPLAYER>& GetPlayerIDs() { return mPlayerSessionIDs; }
	int GetPlayerCnt() { return mPlayerCnt; }
	int GetPlayerLimit() { return mPlayerLimit; }
	GameCode GetGameMode() { return mGameMode; }
	Map& GetMap() { return mMap; }

	void AddPlayerCnt() { mPlayerCnt++; }
	void SetPlayerLimit(int num) { mPlayerLimit = num; }
	void SetGameMode(GameCode gamecode) { mGameMode = gamecode; }

private:

	GameCode mGameMode;
	Map mMap;

	std::mutex mPlayerListLock;
	std::array<Player*, MAXPLAYER> mPlayerList;

	std::mutex mPlayerSessionIDsLock;
	std::array<int, MAXPLAYER> mPlayerSessionIDs;

	int mRoomID;
	int mPlayerCnt = 0;
	int mPlayerLimit = 0;
};

