#pragma once
#include"../GameManager.h"

class FITH : public GameManager
{
public:
	FITH() {}
	FITH(class Server* server, GameMode gameMode);

	~FITH() {}

	virtual void StartGame(int roomID) override;
	virtual void CheckGameEnd(int roomID) override;
	virtual void TimeoverGameEnd(int roomID) override;
	virtual int CalculatePoint(sPlayerGameRecord record, bool isWin) override;
	virtual int CalculateGoldReward(int point, bool isMvp, bool isWin) override;
	virtual void CalculateGameResult(int roomID, std::set<int>& winningTeams) override;

	// Bomb, Weapon Spawn Position
	virtual std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount) override;
	virtual void BombSpawn(class Room* room, int roomID) override;
	virtual void WeaponSpawn(class Room* room, int roomID, eWeaponType weaponType, int spawnCount) override;

private:
	class Server* mServer = nullptr;
	class DB* mDB = nullptr;
	class Timer* mTimer = nullptr;
	class TableManager* mTableManager = nullptr;
	class PacketSender* mPacketSender = nullptr;

	GameMode mGameMode = GameMode::INVALUE_MODE;
};

