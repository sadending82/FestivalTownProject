#pragma once
#include "../../utility.h"
#include "../../protocol.h"
#include "../flatbuffer/FlatBufferManager.h"
#include <unordered_map>

class PacketMaker
{
public:
	PacketMaker() {};
	~PacketMaker() {};

	std::vector<uint8_t> MakePlayerAddPacket(std::array<class Player*, MAXPLAYER>& players);
	std::vector<uint8_t> MakePlayerDeletePacket (int inGameID);
	std::vector<uint8_t> MakePlayerDeadPacket(int inGameID, int roomID, Vector3f position, Vector3f direction);
	std::vector<uint8_t> MakePlayerRespawnPacket(int inGameID, int roomID, Vector3f position, int hp);
	std::vector<uint8_t> MakePlayerCalculatedDamagePacket(int targetID, int attackType, int hp, int damageAmount, Vector3f knockback_direction);

	std::vector<uint8_t> MakeGameMatchingResponsePacket(int inGameID, int roomID, int team, int gameMode, int totalPlayerCount, bool isHost = false);
	std::vector<uint8_t> MakeAllPlayerReadyPacket();
	std::vector<uint8_t> MakeGameStartPacket(int roomid, long time = 0);

	std::vector<uint8_t> MakeHeartBeatPacket();
	std::vector<uint8_t> MakeBlockDropPacket(int x, int y, int type);
	std::vector<uint8_t> MakeBombSpawnPacket(Vector3f position, int bombid);
	std::vector<uint8_t> MakeWeaponSpawnPacket(Vector3f positon, int weaponid, int weaponType);
	std::vector<uint8_t> MakeWeaponDropPacket(Vector3f position, int weaponid);
	std::vector<uint8_t> MakeBombExplosionPacket(int bombID, Vector3f position);
	std::vector<uint8_t> MakeGameStartPacket(int roomID, int startTime);
	std::vector<uint8_t> MakeGameEndPacket(uint8_t winningTeams_flag);
	std::vector<uint8_t> MakeGameResultPacket(std::set<int>& winningTeams, std::unordered_map<int, sPlayerGameRecord>& records, std::array<class Player*, MAXPLAYER>& players);
	std::vector<uint8_t> MakeGameHostChangePacket(int inGameID, int roomID);

	std::vector<uint8_t> MakeLifeReducePacket(int team, int lifeCount);
	std::vector<uint8_t> MakeRemainTimeSyncPacket(int roomID, TIMEPOINT time, int gameEndTime);

private:

};

