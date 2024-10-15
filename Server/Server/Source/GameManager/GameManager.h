#pragma once
#include "../utility.h"
#include "../protocol.h"
#include <set>


class GameManager
{
public:
	GameManager() {}
	GameManager(class Server* server, GameMode gameMode);

	~GameManager() {}

	virtual void StartGame(int roomID) {}
	virtual void CheckGameEnd(int roomID) {}
	virtual void TimeoverGameEnd(int roomID) {}
	virtual int CalculatePoint(sPlayerGameRecord record, bool isWin) { return 0; }
	virtual int CalculateGoldReward(int point, bool isMvp, bool isWin) { return 0; }
	virtual void CalculateGameResult(int roomID, std::set<int>& winningTeams) {}

	virtual bool DeletePlayer(int playerID, int roomID) { return false; }

	virtual std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount) { return std::set<Vector3f>(); }
	virtual void BombSpawn(class Room* room, int roomID) {}
	virtual void WeaponSpawn(class Room* room, int roomID, eWeaponType weaponType, int spawnCount) {}

	virtual bool CheckValidPlayerPosition(Vector3f& position) { return false; }


private:
	class Server* mServer = nullptr;
	class DB* mDB = nullptr;
	class Timer* mTimer = nullptr;
	class TableManager* mTableManager = nullptr;
	class PacketSender* mPacketSender = nullptr;

	GameMode mGameMode = GameMode::INVALUE_MODE;
};

