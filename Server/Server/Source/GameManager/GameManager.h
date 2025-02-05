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

	virtual void PrepareForStartGame(class Room* room, int roomID) {};
	virtual void StartGame(int roomID) {}
	virtual bool CheckGameEnd(int roomID) { return false; }
	virtual bool TimeoverGameEnd(int roomID) { return false; }
	virtual int CalculatePoint(sPlayerGameRecord& record, BattleResult result) { return 0; }
	virtual std::vector<sGameReward> CalculateGameReward(int point, bool isMvp, BattleResult result) { return std::vector<sGameReward>(); }
	virtual void CalculateGameResult(int roomID, std::set<int>& winningTeams) {}
	virtual void UpdateMissionbyGameRecord(class Player* player, bool isWin, struct sPlayerGameRecord& gameRecord) {}

	virtual bool DeletePlayer(int playerID, int roomID) { return false; }

	virtual std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount) { return std::set<Vector3f>(); }
	virtual void BombSpawn(class Room* room, int roomID) {}
	virtual void WeaponSpawn(class Room* room, int roomID, eWeaponType weaponType, int spawnCount) {}
	virtual void PlayerSpawn(class Room* room, int roomID, class Player* player) {}
	virtual bool PlayerDamageReceive(int roomID, class Room* room, int attackerID, class Player* attacker, int targetID, class Player* target
		, Vector3f knockback_direction, eDamageType damageType) { return false; }

	virtual bool CheckValidPlayerPosition(Vector3f& position) { return false; }

private:
	class Server* mServer = nullptr;
	class DB* mDB = nullptr;
	class Timer* mTimer = nullptr;
	class TableManager* mTableManager = nullptr;
	class PacketSender* mPacketSender = nullptr;

	GameMode mGameMode = GameMode::INVALUE_MODE;
};

