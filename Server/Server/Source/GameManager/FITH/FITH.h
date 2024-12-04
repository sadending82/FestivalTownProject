#pragma once
#include"../GameManager.h"

class FITH : public GameManager
{
public:
	FITH() {}
	FITH(class Server* server, GameMode gameMode);

	~FITH() {}

	virtual void PrepareForStartGame(class Room* room, int roomID) override;
	virtual void StartGame(int roomID) override;
	virtual bool CheckGameEnd(int roomID) override;
	virtual bool TimeoverGameEnd(int roomID) override;
	virtual int CalculatePoint(sPlayerGameRecord& record, BattleResult result) override;
	virtual int CalculateGoldReward(int point, bool isMvp, BattleResult result) override;
	virtual void CalculateGameResult(int roomID, std::set<int>& winningTeams) override;

	virtual bool DeletePlayer(int playerID, int roomID) override;

	// Bomb, Weapon Spawn Position
	virtual std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount) override;
	virtual void BombSpawn(class Room* room, int roomID) override;
	virtual void WeaponSpawn(class Room* room, int roomID, eWeaponType weaponType, int spawnCount) override;

	virtual bool PlayerDamageReceive(int roomID, class Room* room, int attackerID, class Player* attacker, int targetID, class Player* target
		, Vector3f knockback_direction, eDamageType damageType) override;
	bool PlayerDieInstant(int roomID, Room* room, int targetID, Player* target);
	bool PlayerDamagedFromOther(int roomID, Room* room, int attackerID, Player* attacker, int targetID, Player* target, Vector3f knockback_direction, eDamageType damageType);
	bool PlayerDamagedFromBomb(int roomID, Room* room, int targetID, Player* target, Vector3f knockback_direction);


	virtual bool CheckValidPlayerPosition(Vector3f& position) override;

private:
	class Server* pServer = nullptr;
	class DB* pDB = nullptr;
	class Timer* pTimer = nullptr;
	class TableManager* pTableManager = nullptr;
	class PacketSender* pPacketSender = nullptr;

	GameMode mGameMode = GameMode::INVALUE_MODE;
};

