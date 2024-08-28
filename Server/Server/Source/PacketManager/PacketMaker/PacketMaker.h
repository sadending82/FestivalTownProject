#pragma once
#include "../../utility.h"
#include "../../protocol.h"
#include "../flatbuffer/FlatBufferManager.h"

class PacketMaker
{
public:
	PacketMaker() {};
	~PacketMaker() {};

	std::vector<uint8_t> MakePlayerAdd(int inGameID);
	std::vector<uint8_t> MakePlayerRespawnPacket(int inGameID, int roomID, int x, int y);
	std::vector<uint8_t> MakeGameMatchingResponsePacket(int inGameID, int roomID, int team, int gameMode, int totalPlayerCount, bool isHost = false);
	std::vector<uint8_t> MakeAllPlayerReadyPacket();
	std::vector<uint8_t> MakeGameStartPacket(int roomid, long time = 0);

	std::vector<uint8_t> MakeHeartBeatPacket();
	std::vector<uint8_t> MakeBlockDropPacket(int x, int y, int type);
	std::vector<uint8_t> MakeBombSpawnPacket(int x, int y, int bombid);
	std::vector<uint8_t> MakeBombExplosionPacket(int bombID, Vector3f position);
	std::vector<uint8_t> MakeGameStartPacket(int roomID, int startTime);
	std::vector<uint8_t> MakeGameEndPacket(uint8_t winningTeams_flag);
	std::vector<uint8_t> MakeGameHostChangePacket(int inGameID, int roomID);

	std::vector<uint8_t> MakeLifeReducePacket(int team, int lifeCount);
	std::vector<uint8_t> MakeRemainTimeSyncPacket(int roomID, TIMEPOINT time, int gameEndTime);

private:

};

