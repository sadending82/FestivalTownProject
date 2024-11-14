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
	virtual int CalculatePoint(sPlayerGameRecord& record, BattleResult result) override;
	virtual int CalculateGoldReward(int point, bool isMvp, BattleResult result) override;
	virtual void CalculateGameResult(int roomID, std::set<int>& winningTeams) override;

	virtual bool DeletePlayer(int playerID, int roomID) override;

	// Bomb, Weapon Spawn Position
	virtual std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount) override;
	virtual void BombSpawn(class Room* room, int roomID) override;
	virtual void WeaponSpawn(class Room* room, int roomID, eWeaponType weaponType, int spawnCount) override;

	virtual bool CheckValidPlayerPosition(Vector3f& position) override;

private:
	class Server* pServer = nullptr;
	class DB* pDB = nullptr;
	class Timer* pTimer = nullptr;
	class TableManager* pTableManager = nullptr;
	class PacketSender* pPacketSender = nullptr;

	GameMode mGameMode = GameMode::INVALUE_MODE;
};

