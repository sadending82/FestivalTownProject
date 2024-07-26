#pragma once
#include "../utility.h"
#include "../Object/Player.h"
class Room
{
public:

	std::vector<Player*>& GetPlayerList() { return mPlayerList; }

private:

	std::mutex mPlayerListLock;
	std::vector<Player*> mPlayerList;

	std::mutex mPlayerIDsLock;
	std::vector<int> mPlayerIDs;
};

