#pragma once
#include "../../utility.h"
#include "../../protocol.h"
#include "../PacketProcessors/flatbuffer/FlatBufferManager.h"

class PacketMaker
{
public:
	PacketMaker() {};
	~PacketMaker() {};

	std::vector<uint8_t> MakePlayerAdd(int inGameID);
	std::vector<uint8_t> MakePlayerGameInfo(int inGameID, int roomID);
	std::vector<uint8_t> MakeHeartBeatPacket();
	std::vector<uint8_t> MakeObjectDropPacket(int x, int y, int type);
	std::vector<uint8_t> MakeBombSpawnPacket(int x, int y);

	std::vector<uint8_t> MakeLifeReducePacket(int team, int lifeCount);
	std::vector<uint8_t> MakeRemainTimeSyncPacket(int roomID, TIMEPOINT time, int gameEndTime);


private:

};

