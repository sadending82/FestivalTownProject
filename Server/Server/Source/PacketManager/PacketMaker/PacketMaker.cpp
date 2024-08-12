#pragma once
#include "PacketMaker.h"

std::vector<uint8_t> PacketMaker::MakePlayerAdd(int inGameID)
{
	mBuilder.Clear();
	mBuilder.Finish(PacketTable::PlayerTable::CreatePlayerAdd(mBuilder, inGameID));
	return MakeBuffer(ePacketType::S2C_PLAYERADD, mBuilder.GetBufferPointer(), mBuilder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerGameInfo(int inGameID, int roomID)
{
	mBuilder.Clear();
	mBuilder.Finish(PacketTable::PlayerTable::CreatePlayerGameInfo(mBuilder, inGameID, roomID));
	return MakeBuffer(ePacketType::S2C_PLAYERGAMEINFO, mBuilder.GetBufferPointer(), mBuilder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeHeartBeatPacket()
{
	mBuilder.Clear();
	long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	mBuilder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(mBuilder, currTime));
	return MakeBuffer(ePacketType::S2C_HEARTBEAT, mBuilder.GetBufferPointer(), mBuilder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeObjectDropPacket(int x, int y, int type)
{
	mBuilder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec2(mBuilder, x, y);
	mBuilder.Finish(PacketTable::ObjectTable::CreateObjectDrop(mBuilder, pos, type));
	return MakeBuffer(ePacketType::S2C_OBJECTDROP, mBuilder.GetBufferPointer(), mBuilder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombSpawnPacket(int x, int y)
{
	mBuilder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec2(mBuilder, x, y);
	mBuilder.Finish(PacketTable::ObjectTable::CreateBombSpawn(mBuilder, pos));
	return std::vector<uint8_t>();
}

