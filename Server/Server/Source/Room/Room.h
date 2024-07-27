#pragma once
#include "../utility.h"
#include "../Object/Player.h"

class Room
{
public:

	Room();
	~Room();

	void Init(int id);

	void addPlayer(Player* player);

	std::array<Player*, MAXPLAYER>& GetPlayerList() { return mPlayerList; }
	std::array<int, MAXPLAYER>& GetPlayerIDs() { return mPlayerSessionIDs; }

private:

	std::mutex mPlayerListLock;
	std::array<Player*, MAXPLAYER> mPlayerList;

	std::mutex mPlayerSessionIDsLock;
	std::array<int, MAXPLAYER> mPlayerSessionIDs;

	int mRoomID;
};

